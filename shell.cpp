#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "shell.h"
#include "array_buffer_allocator.h"

#include "znode.h"
#include "zsprite_node.h"
#include "zscene_node.h"

#include "v8pp/object.hpp"
#include "v8pp/module.hpp"
#include "v8pp/class.hpp"

using namespace v8;
using namespace moodycamel;
using json = nlohmann::json;

v8::Local<v8::Object>       Shell::_self;
v8::Isolate*                Shell::_isolate;
v8::Persistent<v8::Context> Shell::_global_context;
v8::Platform*               Shell::_platform;

void Shell::LoadMainScript(const std::string& file_name, v8::Local<v8::Context>& context) {
    v8::Local<v8::String> name(v8::String::NewFromUtf8(context->GetIsolate(),
                               file_name.c_str(),
                               v8::NewStringType::kNormal).ToLocalChecked());

    v8::Local<v8::String> src;
    Shell::ReadFile(_isolate, file_name.c_str()).ToLocal(&src);
    Shell::ExecuteString(_isolate, src, name, false, false);
}

Shell::Shell(int argc, char *argv[]) {
    v8::V8::InitializeICU();
    v8::V8::InitializeExternalStartupData(argv[0]);
    _platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(_platform);
    v8::V8::Initialize();
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    ArrayBufferAllocator array_buffer_allocator;
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = &array_buffer_allocator;
    _isolate = v8::Isolate::New(create_params);
    _isolate->Enter();
    int result;
    {
        v8::Isolate::Scope isolate_scope(_isolate);
        v8::HandleScope handle_scope(_isolate);
        CreateShellContext();
        if (Context().IsEmpty()) {
            throw std::runtime_error("Error creating context");
        }
    }

    _debugger_thread = std::thread(&Shell::DebuggerThread, this);

}

Shell::~Shell() {
    _isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete _platform;
}

GlobalFunction Shell::GetGlobalFunction(const std::string& functionName) {
    Handle<Object> global = _isolate->GetCurrentContext()->Global();
    Handle<Value> value = global->Get(
            String::NewFromUtf8(_isolate, functionName.c_str()));
    assert(value->IsFunction());
    return GlobalFunction(_isolate, Handle<Function>::Cast(value));
}

void Shell::CreateShellContext() {

    v8::Local<v8::Context> ctxt = v8::Context::New(_isolate);
    _global_context.Reset(_isolate, ctxt);
    v8::Context::Scope context_scope(ctxt);

    Handle<ObjectTemplate> t = ObjectTemplate::New();
    t->SetInternalFieldCount(1);
    _self = t->NewInstance();
    _self->SetInternalField(0, External::New(_isolate, this));

    ctxt->Global()->Set(
            v8::String::NewFromUtf8(_isolate, "print"),
            v8::FunctionTemplate::New(_isolate, Print, _self)->GetFunction());    

    ctxt->Global()->Set(
            v8::String::NewFromUtf8(_isolate, "read"),
            v8::FunctionTemplate::New(_isolate, Read, _self)->GetFunction());    

    ctxt->Global()->Set(
            v8::String::NewFromUtf8(_isolate, "load"),
            v8::FunctionTemplate::New(_isolate, Load, _self)->GetFunction());    

    ctxt->Global()->Set(
            v8::String::NewFromUtf8(_isolate, "quit"),
            v8::FunctionTemplate::New(_isolate, Quit, _self)->GetFunction());    

    ctxt->Global()->Set(
            v8::String::NewFromUtf8(_isolate, "version"),
            v8::FunctionTemplate::New(_isolate, Version, _self)->GetFunction());    



    v8pp::class_<glm::vec2> vec2_class(_isolate);
    vec2_class
        .ctor<float, float>()
        .set("x", &glm::vec2::x)
        .set("y", &glm::vec2::y)
        ;

    v8pp::class_<glm::vec3> vec3_class(_isolate);
    vec3_class
        .ctor<float, float, float>()
        .set("x", &glm::vec3::x)
        .set("y", &glm::vec3::y)
        .set("z", &glm::vec3::z)
        ;

    v8pp::class_<glm::vec4> vec4_class(_isolate);
    vec4_class
        .ctor<float, float, float, float>()
        .set("x", &glm::vec4::x)
        .set("y", &glm::vec4::y)
        .set("z", &glm::vec4::z)
        .set("w", &glm::vec4::w)
        ;

    v8pp::class_<ZNode> znode_class(_isolate);
    znode_class
        .ctor<>()
        .set("position", v8pp::property(&ZNode::get_position, &ZNode::set_position))
        ;

    v8pp::class_<ZSpriteNode> zsprite_node_class(_isolate);
    zsprite_node_class
        .ctor<>()
        .inherit<ZNode>()
        .set("color", &ZSpriteNode::color)
        ;

    v8pp::class_<ZSceneNode> zscene_node_class(_isolate);
    zscene_node_class
        .ctor<>()
        .inherit<ZNode>()
        .set("backgroundColor",
                v8pp::property(
                    &ZSceneNode::get_background_color,
                    &ZSceneNode::set_background_color))
        ;

    v8pp::module m(_isolate);
    m
        .set("Vec2", vec2_class)
        .set("Vec3", vec3_class)
        .set("Vec4", vec4_class)
        .set("Node", znode_class)
        .set("SpriteNode", zsprite_node_class)
        .set("SceneNode", zscene_node_class)
        ;

    ctxt->Global()->Set(
            v8::String::NewFromUtf8(_isolate, "ZEngine"),
            m.new_instance());

    LoadMainScript("main.js", ctxt);

    GlobalFunction _init_function = GetGlobalFunction("init");
    GlobalFunction _update_function = GetGlobalFunction("update");
    GlobalFunction _shutdown_function = GetGlobalFunction("shutdown");
}

ZNode::ZNode() : pos_(0,0) {
    v8pp::class_<glm::vec2>::reference_external(Shell::_isolate, &pos_);
}

ZNode::~ZNode() {
    // v8pp::class_<glm::vec2>::unreference_external(Shell::_isolate, &pos_);
}

ZSpriteNode::ZSpriteNode() {
    // v8pp::class_<glm::vec4>::reference_external(Shell::_isolate, &color_);
}

ZSpriteNode::~ZSpriteNode() {
    // v8pp::class_<glm::vec4>::unreference_external(Shell::_isolate, &color_);
}

ZSceneNode::ZSceneNode() : background_color_(0,0,0,0), temp("booga") {
    v8pp::class_<glm::vec4>::reference_external(Shell::_isolate, &background_color_);
}

ZSceneNode::~ZSceneNode() {
    v8pp::class_<glm::vec4>::unreference_external(Shell::_isolate, &background_color_);
}

ZSceneNode* Shell::Init() {
    v8::Context::Scope context_scope(Context());
    HandleScope scope(_isolate);
    GlobalFunction _update_function = GetGlobalFunction("init");
    Local<Function> func = Local<Function>::New(_isolate, _update_function);
    Handle<Value> args[1];
    Handle<Value> result = func->Call(Context()->Global(), 0, args);

    ZSceneNode *scene = &v8pp::from_v8<ZSceneNode>(_isolate, result);
    std::cout << "scene->get_background_color().x: " << scene->get_background_color().x << std::endl;
    std::cout << "scene->get_background_color().y: " << scene->get_background_color().y << std::endl;
    std::cout << "scene->get_background_color().z: " << scene->get_background_color().z << std::endl;
    std::cout << "scene->get_background_color().w: " << scene->get_background_color().w << std::endl;

    return scene;
}

void Shell::Poll() {
    while (true) {
        std::string jsExpression;
        bool succeeded = UnevaluatedQueue.try_dequeue(jsExpression);
        if (!succeeded) break;
        std::cout << "expression to evaluate: " << jsExpression << std::endl;
        std::string evaluation(Eval(jsExpression));
        std::cout << "evaluation: " << evaluation << std::endl;
        EvaluatedQueue.enqueue(evaluation);
    }
}

std::string Shell::Eval(std::string jsExpression) {
    Context::Scope context_scope(Context());
    HandleScope handle_scope(_isolate);
    TryCatch try_catch(_isolate);
    Local<Script> script;
    Local<String> source = String::NewFromUtf8(_isolate,
                                               jsExpression.c_str(),
                                               NewStringType::kNormal).ToLocalChecked();
    if (!Script::Compile(Context(), source).ToLocal(&script)) {
        ReportException(_isolate, &try_catch);
        return "oops. compile error";;
    }
    Local<Value> result;
    if (!script->Run(Context()).ToLocal(&result)) {
        assert(try_catch.HasCaught());
        ReportException(_isolate, &try_catch);
        return "oops. runtime error";
    }
    assert(!try_catch.HasCaught());
    String::Utf8Value str(result);

    std::string s(ToCString(str));
    return s;
}

bool Shell::Update(const float elapsedTime) {
    v8::Context::Scope context_scope(Context());
    HandleScope scope(_isolate);
    GlobalFunction _update_function = GetGlobalFunction("update");
    Local<Function> func = Local<Function>::New(_isolate, _update_function);
    Handle<Value> args[1];
    args[0] = Number::New(_isolate, elapsedTime);
    Handle<Value> result = func->Call(Context()->Global(), 1, args);

    return !result->IsBoolean() || result->BooleanValue();
}

void Shell::Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    for (int i(0); i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::String::Utf8Value str(args[i]);
        std::string s(ToCString(str));
        std::cout << s << std::endl;
    }
}

void Shell::Read(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (args.Length() != 1) {
        args.GetIsolate()->ThrowException(
                v8::String::NewFromUtf8(args.GetIsolate(),
                                        "Bad parameters",
                                        v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::String::Utf8Value file(args[0]);
    if (*file == NULL) {
        args.GetIsolate()->ThrowException(
                v8::String::NewFromUtf8(args.GetIsolate(),
                                        "Error loading file",
                                        v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    v8::Local<v8::String> source;
    if (!ReadFile(args.GetIsolate(), *file).ToLocal(&source)) {
        args.GetIsolate()->ThrowException(
                v8::String::NewFromUtf8(args.GetIsolate(),
                                        "Error loading file",
                                        v8::NewStringType::kNormal).ToLocalChecked());
        return;
    }
    args.GetReturnValue().Set(source);
}

void Shell::Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
    for (int i(0); i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::String::Utf8Value file(args[i]);
        if (*file == NULL) {
            args.GetIsolate()->ThrowException(
                    v8::String::NewFromUtf8(args.GetIsolate(),
                                            "Error loading file",
                                            v8::NewStringType::kNormal).ToLocalChecked());
            return;
        }
        v8::Local<v8::String> source;
        if (!ReadFile(args.GetIsolate(), *file).ToLocal(&source)) {
            args.GetIsolate()->ThrowException(
                    v8::String::NewFromUtf8(args.GetIsolate(),
                                            "Error loading file",
                                            v8::NewStringType::kNormal).ToLocalChecked());
            return;
        }
        if (!ExecuteString(args.GetIsolate(), source, args[i], false, false)) {
            args.GetIsolate()->ThrowException(
                    v8::String::NewFromUtf8(args.GetIsolate(),
                                            "Error executing file",
                                            v8::NewStringType::kNormal).ToLocalChecked());
            return;
        }
    }
}

void Shell::Quit(const v8::FunctionCallbackInfo<v8::Value>& args) {
    int exit_code =
        args[0]->Int32Value(args.GetIsolate()->GetCurrentContext()).FromMaybe(0);
    exit(exit_code);
}

void Shell::Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().Set(
            v8::String::NewFromUtf8(args.GetIsolate(),
                                    v8::V8::GetVersion(),
                                    v8::NewStringType::kNormal).ToLocalChecked());
}

v8::MaybeLocal<v8::String> Shell::ReadFile(v8::Isolate* isolate, const char* name) {
    std::ifstream file(name);
    std::string str;
    std::string file_contents;
    while (std::getline(file, str)) {
        file_contents += str + "\n";
    }
    v8::MaybeLocal<v8::String> result = v8::String::NewFromUtf8(
            isolate,
            file_contents.c_str(),
            v8::NewStringType::kNormal,
            static_cast<int>(file_contents.size()));
    return result;
}

bool Shell::ExecuteString(v8::Isolate* isolate,
                   v8::Local<v8::String> source,
                   v8::Local<v8::Value> name,
                   bool print_result,
                   bool report_exceptions) {
    v8::HandleScope handle_scope(isolate);
    v8::TryCatch try_catch(isolate);
    v8::ScriptOrigin origin(name);
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    v8::Local<v8::Script> script;
    if (!v8::Script::Compile(context, source, &origin).ToLocal(&script)) {
        if (report_exceptions)
            ReportException(isolate, &try_catch);
        return false;
    }
    else {
        v8::Local<v8::Value> result;
        if (!script->Run(context).ToLocal(&result)) {
            assert(try_catch.HasCaught());
            if (report_exceptions)
                ReportException(isolate, &try_catch);
            return false;
        }
        else {
            assert(!try_catch.HasCaught());
            if (print_result && !result->IsUndefined()) {
                v8::String::Utf8Value str(result);
                std::string s(ToCString(str));
                std::cout << s << std::endl;
            }
            return true;
        }
    }
}

void Shell::ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value exception(try_catch->Exception());
    std::string exception_string(ToCString(exception));
    v8::Local<v8::Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        std::cout << exception_string << std::endl;
    }
    else {
        v8::String::Utf8Value filename(message->GetScriptOrigin().ResourceName());
        v8::Local<v8::Context> context(isolate->GetCurrentContext());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber(context).FromJust();
        std::cout << filename_string << ":" << linenum << ": " << exception_string << std::endl;

        v8::String::Utf8Value sourceline(
                message->GetSourceLine(context).ToLocalChecked());
        std::string sourceline_string(ToCString(sourceline));
        std::cout << sourceline_string << std::endl;

        int start = message->GetStartColumn(context).FromJust();
        for (int i(0); i < start; i ++) {
            std::cout << " ";
        }
        int end = message->GetEndColumn(context).FromJust();
        for (int i(start); i < end; i ++) {
            std::cout << "^";
        }
        std::cout << std::endl;
        v8::Local<v8::Value> stack_trace_string;
        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
                stack_trace_string->IsString() &&
                v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
            v8::String::Utf8Value stack_trace(stack_trace_string);
            std::string stack_trace_string(ToCString(stack_trace));
            std::cout << stack_trace_string << std::endl;
        }

    }

}

namespace {
    const int debugging_port = 5858;
    const char* const kContentLength = "Content-Length";
    const int kContentLengthSize = strlen(kContentLength);

    void SendBuffer(int socket, const std::string& message) {
        std::string payload = message;    
        if (payload.empty())
            payload = "(empty)";
        int n = send(socket, payload.c_str(), payload.size(), 0);
    }

    void SendMessage(int conn, const std::string& message) {
        SendBuffer(conn, message); 
    }

    std::string GetRequest(int socket) {
        int received;
        int content_length(0);
        while (true) {
            const int kHeaderBufferSize(80);
            char header_buffer[kHeaderBufferSize];
            int header_buffer_position(0);
            char c =  '\0';

            while(c != '\n') {
                received = recv(socket, &c, 1, 0);
                //assert(received >= 0);
                if (header_buffer_position < kHeaderBufferSize)
                    header_buffer[header_buffer_position++] = c;
            }

            if (header_buffer_position == 1)
                break;

            assert(header_buffer_position > 0);
            assert(header_buffer_position <= kHeaderBufferSize);
            header_buffer[header_buffer_position - 1] = '\0';

            char* key = header_buffer;
            char* value = nullptr;
            for (int i(0); header_buffer[i] != '\0'; i++)
                if (header_buffer[i] == ':') {
                    header_buffer[i] = '\0';
                    value = header_buffer + i + 1;
                    while (*value == ' ')
                        value++;
                    break;
                }

            if (strcmp(key, kContentLength) == 0) {
                if (value == nullptr || strlen(value) > 7)
                    return std::string();
                for (int i = 0; value[i] != '\0'; i++) {
                    if (value[i] < '0' || value[i] > '9')
                        return std::string();
                    content_length = 10 * content_length + (value[i] - '0');
                }
            }
            else
                std::cout << key
                          << ": "
                          << (value != nullptr ? value : "(no value)")
                          << std::endl;
        }

        if (content_length == 0)
            return std::string();

        std::string buffer;
        buffer.resize(content_length);
        received = recv(socket, &buffer[0], content_length, 0);
        if (received < content_length) {
            std::cout << "Error request data size" << std::endl;
            return std::string();
        }
        buffer[content_length] = '\0';
        return buffer;
    }
}

int Shell::_main_debug_client_socket = -1;

void Shell::DebuggerThread() {
    std::cout << "Begin debugger thread" << std::endl;
    
    int sockfd, client_socket, portno;
    socklen_t clilen;
    sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    int yes=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    portno = debugging_port;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;;
    serv_addr.sin_port = htons(portno);
    int bindResult = bind(sockfd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr));

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        client_socket = accept(sockfd, reinterpret_cast<sockaddr *>(&cli_addr), &clilen);
        assert(client_socket >= 0);
        _main_debug_client_socket = client_socket;
        std::cout << "Client connected to debugger" << std::endl;

        SendBuffer(client_socket, "Type: connect\n");
        SendBuffer(client_socket,
                std::string("V8-Version: ") + std::string(V8::GetVersion()) + std::string("\n"));
        SendBuffer(client_socket, "Protocol-Version: 1\n");
        SendBuffer(client_socket, std::string("Embedding-Host: ZEngine\n"));
        SendBuffer(client_socket, "Content-Length: 0\n");
        SendBuffer(client_socket, "\n");

        while (1) {
            std::string request = GetRequest(client_socket);

            if (request.empty())
                continue;

            json j_request = json::parse(request);
            std::string jsExpression = j_request["arguments"]["expression"];

            UnevaluatedQueue.enqueue(jsExpression);
            while (true) {
                std::string evaluated;
                bool succeeded = EvaluatedQueue.try_dequeue(evaluated);
                if (succeeded) {
                    SendMessage(client_socket, evaluated);
                    break;
                }
            }

        }

    }

}

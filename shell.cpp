#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

#include "shell.h"
#include "array_buffer_allocator.h"

#include "script_object_wrap.h"
#include "script_vector_3.h"

using namespace v8;

void Shell::LoadMainScript(const std::string& file_name, v8::Local<v8::Context>& context) {
    v8::Local<v8::String> name(v8::String::NewFromUtf8(context->GetIsolate(),
                               file_name.c_str(),
                               v8::NewStringType::kNormal).ToLocalChecked());

    v8::Local<v8::String> src;
    Shell::ReadFile(_isolate, file_name.c_str()).ToLocal(&src);
    Shell::ExecuteString(_isolate, src, name, false, false);
}

Shell::Shell(int argc, char *argv[]) {
    std::cout << "ONE" << std::endl;
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
        // v8::Context::Scope context_scope(Context());
        // RunShell(context, _platform);
    }
}

void Shell::CleanUp() {
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


    LoadMainScript("test_script.js", ctxt);

    ScriptVector3::Create(_isolate, ctxt);

    GlobalFunction _init_function = GetGlobalFunction("init");
    GlobalFunction _update_function = GetGlobalFunction("update");
    GlobalFunction _shutdown_function = GetGlobalFunction("shutdown");
}

void Shell::Init() {
    v8::Context::Scope context_scope(Context());
    HandleScope scope(_isolate);
    GlobalFunction _update_function = GetGlobalFunction("init");
    Local<Function> func = Local<Function>::New(_isolate, _update_function);
    Handle<Value> args[1];
    Handle<Value> result = func->Call(Context()->Global(), 0, args);
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

void completion(const char *buf, linenoiseCompletions *lc) {
    switch(buf[0]) {
        case 'q': {
            linenoiseAddCompletion(lc, "quit()");
            break;
        }
        case 'l': {
            linenoiseAddCompletion(lc, "load()");
            break;
        }
        case 'r': {
            linenoiseAddCompletion(lc, "read()");
            break;
        }
        case 'p': {
            linenoiseAddCompletion(lc, "print()");
            break;
        }
        case 'v': {
            linenoiseAddCompletion(lc, "version()");
            break;
        }
        default:
            break;
    }
}

void Shell::RunShell(v8::Local<v8::Context> context,
              v8::Platform* platform) {
    std::cout << "V8 version " << v8::V8::GetVersion() << " for ZEngine version 0.1" << std::endl;
    std::cout << "Type quit() or press CTRL-C to quit" << std::endl;
    static const int kBufferSize = 256;
    v8::Local<v8::String> name(
            v8::String::NewFromUtf8(context->GetIsolate(),
                                    "(shell)",
                                    v8::NewStringType::kNormal).ToLocalChecked());

    char *line;
    linenoiseSetCompletionCallback(completion);
    while (line = linenoise("ZEngine> ")) {
        linenoiseHistoryAdd(line);
        v8::HandleScope handle_scope(context->GetIsolate());
        ExecuteString(
                context->GetIsolate(),
                v8::String::NewFromUtf8(context->GetIsolate(),
                                        line,
                                        v8::NewStringType::kNormal).ToLocalChecked(),
                name,
                true,
                true);
        while (v8::platform::PumpMessageLoop(_platform, context->GetIsolate()))
            continue;
        free(line);


        return;



    }
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

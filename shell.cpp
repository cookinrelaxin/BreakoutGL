#include <iostream>
#include <sstream>

#include <vector>

#include "shell.h"
#include "array_buffer_allocator.h"

#include "zengine.h"
#include "console.h"
#include "vec2.h"

#include "repl_server.h"

#include <include/libplatform/libplatform.h>

// using namespace v8pp;
using namespace moodycamel;

ReaderWriterQueue<std::string> Shell::UnevaluatedQueue;
ReaderWriterQueue<std::string> Shell::EvaluatedQueue;

v8pp::context* Shell::_context;

void Shell::LoadMainScript(const std::string& file_name) {
    v8::HandleScope scope(_context->isolate());
    _context->run_file(file_name);
}

Shell::Shell() {
    std::cout << "ONE" << std::endl;
    v8::V8::InitializeICU();
    v8::Platform* platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(platform);
    v8::V8::Initialize();
    _context = new v8pp::context;
    // _context->set_lib_path("");
    v8::HandleScope scope(_context->isolate());
    CreateShellContext();

    _debugger_thread = std::thread(&ReplServer::Start);
    std::cout << "TWO" << std::endl;
}

Shell::~Shell() {
    UninitializeV8();
}

void Shell::UninitializeV8() {
    ReplServer::terminateServer = true;
    if (_debugger_thread.joinable()) _debugger_thread.join();
    // v8::Isolate::Scope isolate_scope(_isolate);
    // _isolate->Exit();
    // _isolate->Dispose();
    // v8::V8::Dispose();
    // v8::V8::ShutdownPlatform();
    // delete _platform;
}

void Shell::CreateShellContext() {
    Console::create(_context);
    ZEngine::create(_context);

    LoadMainScript("main.js");
}

ZSceneNode* Shell::Init() {
    v8::Isolate* isolate = _context->isolate();
    v8::HandleScope scope(isolate);
    v8::Handle<v8::Value> result = _context->run_script("init();");
    ZSceneNode *scene = &v8pp::from_v8<ZSceneNode>(isolate, result);
    return scene;
}

void Shell::Poll() {
    while (true) {
        v8::Isolate* isolate = _context->isolate();
        v8::HandleScope scope(isolate);
        std::string jsExpression;
        bool succeeded = UnevaluatedQueue.try_dequeue(jsExpression);
        if (!succeeded) break;
        std::cout << "expression to evaluate: " << jsExpression << std::endl;
        v8::Handle<v8::Value> result = _context->run_script(jsExpression);
        v8::String::Utf8Value str(result);
        std::string evaluation(*str);
        std::cout << "evaluation: " << evaluation << std::endl;
        EvaluatedQueue.enqueue(evaluation);
    }
}

bool Shell::Update(const float elapsedTime) {
    v8::Isolate* isolate = _context->isolate();
    v8::HandleScope scope(isolate);
    bool result = _context->run_script("update()")->BooleanValue();
    // _context->run_script("update()");
    return result;
}

void Shell::KeyDown(int key) {
    v8::Isolate* isolate = _context->isolate();
    v8::HandleScope scope(isolate);
    std::stringstream jsExpression; 
    jsExpression 
        << "key_was_pressed("
        << key
        << ");"
        ;

    _context->run_script(jsExpression.str());
}

void Shell::KeyUp(int key) {
    v8::Isolate* isolate = _context->isolate();
    v8::HandleScope scope(isolate);
    std::stringstream jsExpression; 
    jsExpression 
        << "key_was_released("
        << key
        << ");"
        ;

    _context->run_script(jsExpression.str());
}

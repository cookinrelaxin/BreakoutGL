#include "console.h"

#include <iostream>

void Console::log(v8::FunctionCallbackInfo<v8::Value> const& args) {
    v8::HandleScope handle_scope(args.GetIsolate());

    for (int i(0); i < args.Length(); ++i) {
        if (i > 0) std::cout << ' ';
        v8::String::Utf8Value str(args[i]);
        std::cout << *str;
    }
    std::cout << std::endl;
}

void Console::create(v8::Local<v8::Context> context, v8::Isolate* isolate) {
    v8pp::module m(isolate);
    m.set("log", &log);
    
    context->Global()->Set(
            v8::String::NewFromUtf8(isolate, "console"),
            m.new_instance());
}

#include "require.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "shell.h"

void Require::require(v8::FunctionCallbackInfo<v8::Value> const& args) {
    v8::HandleScope handle_scope(args.GetIsolate());

    std::string path = *static_cast<v8::String::Utf8Value>(args[0]);
    std::ifstream file(path);
    std::string str;
    std::string source;
    while (std::getline(file, str)) {
        source += str;
        source.push_back('\n');
    }

    std::stringstream wrapper;
    wrapper
        << "(function () {"
        << "var module = {"
        << "exports: {}"
        << "}, exports = module.exports;"
        << source
        << "return module.exports"
        << "}());"
        ;

    v8::Handle<v8::Value> result = Shell::_context->run_script(wrapper.str());
    v8::String::Utf8Value result_str(result);
    std::string evaluation(*result_str);

    args.GetReturnValue().Set(result);
}

void Require::create(v8pp::context* context) {
    v8::Isolate* isolate = context->isolate();
    context->set("require", v8::FunctionTemplate::New(isolate, require)->GetFunction());
}


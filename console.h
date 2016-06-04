#ifndef CONSOLE_H
#define CONSOLE_H

#include <v8pp/module.hpp>

class Console {
    public:
        static void log(v8::FunctionCallbackInfo<v8::Value> const& args);
        static void create(v8::Local<v8::Context> context,
                           v8::Isolate* isolate);
};

#endif

#ifndef CONSOLE_H
#define CONSOLE_H

#include <v8pp/module.hpp>
#include <v8pp/context.hpp>

class Console {
    public:
        static void log(v8::FunctionCallbackInfo<v8::Value> const& args);
        static void create(v8pp::context* context);
};

#endif

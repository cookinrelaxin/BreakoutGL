#ifndef REQUIRE_H
#define REQUIRE_H

#include <v8pp/module.hpp>
#include <v8pp/context.hpp>

class Require {
    public:
        static void require(v8::FunctionCallbackInfo<v8::Value> const& args);
        static void create(v8pp::context* context);
};

#endif

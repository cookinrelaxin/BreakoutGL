#ifndef ZENGINE_H
#define ZENGINE_H

#include <v8pp/module.hpp>
#include <v8pp/context.hpp>

class ZEngine {
    public:
        static void create(v8pp::context* context);
};

#endif


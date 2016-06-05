#ifndef VEC2_H
#define VEC2_H

#include <v8pp/class.hpp>

#include <glm/glm.hpp>

class Vec2 {
    public:
        void normalize(v8::FunctionCallbackInfo<v8::Value> const& args);
        static v8pp::class_<glm::vec2> create(v8::Isolate* isolate);
};

#endif

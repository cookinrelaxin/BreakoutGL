#ifndef VEC2_H
#define VEC2_H

#include <v8pp/class.hpp>

#include <glm/glm.hpp>

class Vec2 {
    public:
        static v8pp::class_<glm::vec2> create(v8::Isolate* isolate);
};

#endif

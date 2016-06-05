#ifndef VEC4_H
#define VEC4_H

#include <v8pp/class.hpp>

#include <glm/glm.hpp>

class Vec4 {
    public:
        static v8pp::class_<glm::vec4> create(v8::Isolate* isolate);
};

#endif

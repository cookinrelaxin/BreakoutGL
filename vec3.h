#ifndef VEC3_H
#define VEC3_H

#include <v8pp/class.hpp>

#include <glm/glm.hpp>

class Vec3 {
    public:
        static v8pp::class_<glm::vec3> create(v8::Isolate* isolate);
};

#endif

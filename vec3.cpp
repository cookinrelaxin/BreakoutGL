#include "vec3.h"

v8pp::class_<glm::vec3> Vec3::create(v8::Isolate* isolate) {
    v8pp::class_<glm::vec3> vec3_class(isolate);
    vec3_class
        .ctor<float, float, float>()
        .set("x", &glm::vec3::x)
        .set("y", &glm::vec3::y)
        .set("z", &glm::vec3::z)
        ;
    return vec3_class;
}

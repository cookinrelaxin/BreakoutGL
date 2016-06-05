#include "vec4.h"

v8pp::class_<glm::vec4> Vec4::create(v8::Isolate* isolate) {
    v8pp::class_<glm::vec4> vec4_class(isolate);
    vec4_class
        .ctor<float, float, float, float>()
        .set("x", &glm::vec4::x)
        .set("y", &glm::vec4::y)
        .set("z", &glm::vec4::z)
        .set("w", &glm::vec4::w)
        ;
    return vec4_class;
}

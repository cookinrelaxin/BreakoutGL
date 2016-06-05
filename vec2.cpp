#include "vec2.h"

v8pp::class_<glm::vec2> Vec2::create(v8::Isolate* isolate) {
    v8pp::class_<glm::vec2> vec2_class(isolate);
    vec2_class
        .ctor<float, float>()
        .set("x", &glm::vec2::x)
        .set("y", &glm::vec2::y)
        ;
    return vec2_class;
}

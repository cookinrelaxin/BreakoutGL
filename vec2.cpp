#include "vec2.h"

#include <iostream>

void Vec2::normalize(v8::FunctionCallbackInfo<v8::Value> const& args) {
    v8::HandleScope handle_scope(args.GetIsolate());
    std::cout << "normalize!" << std::endl;
    // glm::vec2 *v = &v8pp::from_v8<glm::vec2>(args.GetIsolate(), args.This());
    // glm::normalize(*v);
}

v8pp::class_<glm::vec2> Vec2::create(v8::Isolate* isolate) {
    v8pp::class_<glm::vec2> vec2_class(isolate);
    vec2_class
        .ctor<float, float>()
        .set("x", &glm::vec2::x)
        .set("y", &glm::vec2::y)
        // .set("normalize", &Vec2::normalize)
        ;
    return vec2_class;
}

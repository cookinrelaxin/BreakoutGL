#include <ray.h>

ray::ray() {}

ray::ray(const glm::vec3& a, const glm::vec3& b)
    : A(a)
    , B(b) {}

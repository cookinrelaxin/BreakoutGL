#include <ray.h>

ray::ray() {}

ray::ray(const glm::vec3& a, const glm::vec3& b)
    : A(a)
    , B(b) {}

glm::vec3 ray::origin() const {
   return A; 
}

glm::vec3 ray::direction() const {
   return B; 
}

glm::vec3 ray::point_at_parameter(float t) const {
   return A + t*B; 
}

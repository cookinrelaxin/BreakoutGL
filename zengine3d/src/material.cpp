#include "material.h"

glm::vec3 material::random_in_unit_sphere() const {
    glm::vec3 p;
    do {
        p = 2.0f * glm::vec3(drand48(), drand48(), drand48()) - glm::vec3(1.0f, 1.0f, 1.0f);
    } while (glm::dot(p,p) >= 1.0f);
    return p;
}

glm::vec3 material::reflect(const glm::vec3& v, const glm::vec3& n) const {
    return v - 2 * glm::dot(v,n) * n;
}

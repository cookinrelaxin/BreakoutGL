#include "metal.h"

bool metal::scatter(const ray& r_in,
                         const hit_record& rec,
                         glm::vec3& attenuation,
                         ray& scattered) const {
    glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
    attenuation = albedo;
    return glm::dot(scattered.direction(), rec.normal) > 0.0f;
}

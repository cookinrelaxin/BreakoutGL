#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "material.h"

class lambertian : public material {
    public:
        lambertian(const glm::vec3& a) : albedo(a) {};

        virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const;

        glm::vec3 albedo;
    private:
};

#endif /* end of include guard: LAMBERTIAN_H */

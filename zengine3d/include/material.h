#ifndef MATERIAL_H
#define MATERIAL_H

#include "ray.h"
#include "hitable.h"

class material {
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const = 0;
    protected:
        glm::vec3 random_in_unit_sphere() const;
        glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) const;
};

#endif /* end of include guard: MATERIAL_H */

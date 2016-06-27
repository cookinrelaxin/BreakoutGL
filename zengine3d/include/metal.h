#ifndef METAL_H
#define METAL_H

#include "material.h"

class metal : public material {
    public:
        metal(const glm::vec3& a, float f)
            : albedo(a)
            , fuzz(f)
        {};

        virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const;

        glm::vec3 albedo;
        float fuzz;
    private:
};

#endif /* end of include guard: METAL_H */


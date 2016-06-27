#ifndef DIALECTRIC_H
#define DIALECTRIC_H

#include "material.h"

class dialectric : public material {
    public:
        dialectric(float ri)
            : ref_idx(ri)
        {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, glm::vec3& attenuation, ray& scattered) const;
        bool refract(const glm::vec3& v,
                     const glm::vec3& n,
                     const float ni_over_nt,
                     glm::vec3& refracted) const;
        float schlick(const float cosine) const;

        const float ref_idx;
};

#endif /* end of include guard: DIALECTRIC_H */

#ifndef HITABLE_H
#define HITABLE_H

#include <ray.h>

class material;

struct hit_record {
    /* data */
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    material* mat_ptr;
} /* optional variable list */;

class hitable {
    public:
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

#endif /* end of include guard: HITABLE_H */

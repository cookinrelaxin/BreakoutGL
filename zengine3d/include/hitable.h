#ifndef HITABLE_H
#define HITABLE_H

#include <ray.h>
#include <aabb.h>

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
        virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

#endif /* end of include guard: HITABLE_H */

#ifndef AABB_H
#define AABB_H

#include "ray.h"

class aabb {
    public:
        aabb() = default;
        aabb(const glm::vec3& a, const glm::vec3& b) 
            : _min(a)
            , _max(b)
        {}

        glm::vec3 min() const;
        glm::vec3 max() const;

        bool hit(const ray& r, float tmin, float tmax) const;

        static aabb surrounding_box(const aabb& box0, const aabb& box1);
    private:
        glm::vec3 _min;
        glm::vec3 _max;

};

#endif /* end of include guard: AABB_H */

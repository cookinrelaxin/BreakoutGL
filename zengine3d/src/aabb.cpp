#include "aabb.h"
#include <iostream>

glm::vec3 aabb::min() const {
    return _min;
}

glm::vec3 aabb::max() const {
    return _max;
}

bool aabb::hit(const ray& r, float tmin, float tmax) const {
    glm::vec3 direction(r.direction());
    glm::vec3 origin(r.origin());
    {
        float invD = 1.0f / direction.x;
        float temp = origin.x * invD;
        float t0 = _min.x * invD - temp;
        float t1 = _max.x * invD - temp;
        if (invD < 0.0f)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
       if (tmax <= tmin) {
           return false;
       }
    }
    {
        float invD = 1.0f / direction.y;
        float temp = origin.y * invD;
        float t0 = _min.y * invD - temp;
        float t1 = _max.y * invD - temp;
        if (invD < 0.0f)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
       if (tmax <= tmin) {
           return false;
       }
    }
    {
        float invD = 1.0f / direction.z;
        float temp = origin.z * invD;
        float t0 = _min.z * invD - temp;
        float t1 = _max.z * invD - temp;
        if (invD < 0.0f)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
       if (tmax <= tmin) {
           return false;
       }
    }
    return true;
}

aabb aabb::surrounding_box(const aabb& box0, const aabb& box1) {
    glm::vec3 small(fmin(box0.min().x, box1.min().x),
                    fmin(box0.min().y, box1.min().y),
                    fmin(box0.min().z, box1.min().z));

    glm::vec3 big(fmax(box0.max().x, box1.max().x),
                  fmax(box0.max().y, box1.max().y),
                  fmax(box0.max().z, box1.max().z));
    return aabb(small, big);
}

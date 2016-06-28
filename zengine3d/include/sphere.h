#ifndef SPHERE_H
#define SPHERE_H

#include <hitable.h>

class sphere : public hitable {
    public:
        sphere();
        sphere(glm::vec3 cen, float r, material* m)
            : center(cen)
            , radius(r)
            , m(m)
    {}
        ~sphere();

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& box) const;

        glm::vec3 center;
        float radius;
        material* m;

    private:
        /* Private Data */
        /* Private Functions */
    protected:
        /* Inherited Data */
        /* Inherited Functions */
};

#endif /* end of include guard: SPHERE_H */

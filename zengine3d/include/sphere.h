#ifndef SPHERE_H
#define SPHERE_H

#include <hitable.h>

class sphere : public hitable {
    public:
        sphere();
        sphere(glm::vec3 cen, float r) : center(cen), radius(r) {}
        ~sphere();

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

        glm::vec3 center;
        float radius;

    private:
        /* Private Data */
        /* Private Functions */
    protected:
        /* Inherited Data */
        /* Inherited Functions */
};

#endif /* end of include guard: SPHERE_H */

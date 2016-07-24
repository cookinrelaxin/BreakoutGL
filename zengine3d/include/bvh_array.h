#ifndef BVH_ARRAY_H
#define BVH_ARRAY_H

#include "hitable.h"

class bvh_array : public hitable {
    public:
        bvh_array(hitable** l, int n, float time0, float time1);
        
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& b) const;

        // hitable* left;
        // hitable* right;
        // aabb box;
        aabb boxes;
};

#endif /* end of include guard: BVH_NODE_H */


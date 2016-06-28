#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "hitable.h"

class bvh_node : public hitable {
    public:
        bvh_node(hitable** l, int n, float time0, float time1);
        
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& b) const;

        hitable* left;
        hitable* right;
        aabb box;
};

#endif /* end of include guard: BVH_NODE_H */

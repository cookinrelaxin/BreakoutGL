#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include "hitable.h"

class hitable_list : public hitable {
    public:
        hitable_list() = default;
        hitable_list(hitable **l, int n) : list(l), list_size(n) {}
        ~hitable_list();

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

        hitable **list;
        int list_size;
};

#endif /* end of include guard: HITABLE_LIST_H */

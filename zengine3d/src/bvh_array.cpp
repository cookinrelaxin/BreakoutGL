#include "bvh_array.h"
#include <stdexcept>
#include <iostream>

int box_x_compare(const void* a, const void* b) {
    aabb box_left, box_right;
    hitable* ah = *(hitable**)a;
    hitable* bh = *(hitable**)b;
    if (   !ah->bounding_box(0.0f, 0.0f, box_left)
        || !bh->bounding_box(0.0f, 0.0f, box_right)) {
        throw std::logic_error("no bounding box in bounding box compare");
    }
    if (box_left.min().x - box_right.min().x < 0.0f) {
       return -1; 
    }
    else {
       return 1; 
    }
}

int box_y_compare(const void* a, const void* b) {
    aabb box_left, box_right;
    hitable* ah = *(hitable**)a;
    hitable* bh = *(hitable**)b;
    if (   !ah->bounding_box(0.0f, 0.0f, box_left)
        || !bh->bounding_box(0.0f, 0.0f, box_right)) {
        throw std::logic_error("no bounding box in bounding box compare");
    }
    if (box_left.min().y - box_right.min().y < 0.0f) {
       return -1; 
    }
    else {
       return 1; 
    }
}

int box_z_compare(const void* a, const void* b) {
    aabb box_left, box_right;
    hitable* ah = *(hitable**)a;
    hitable* bh = *(hitable**)b;
    if (   !ah->bounding_box(0.0f, 0.0f, box_left)
        || !bh->bounding_box(0.0f, 0.0f, box_right)) {
        throw std::logic_error("no bounding box in bounding box compare");
    }
    if (box_left.min().z - box_right.min().z < 0.0f) {
       return -1; 
    }
    else {
       return 1; 
    }
}

bvh_node::bvh_node(bvh_node* bvh, int i) {
    this->boxes[i]. = bvh->
}

bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    if (box.hit(r, t_min, t_max)) {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t) {
                rec = left_rec; 
            }
            else {
                rec = right_rec; 
            }
            return true;
        }
        else if (hit_left) {
            rec = left_rec; 
            return true;
        }
        else if (hit_right) {
            rec = right_rec; 
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}
bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
    b = box;
    return true;
}


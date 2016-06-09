#ifndef Z_VELOCITY_2
#define Z_VELOCITY_2

#include "glm/glm.hpp"

namespace Z {

class velocity2 {
    public:
        velocity2() : dx(0), dy(0) {}
        // velocity2(velocity2& other) : velocity_(other.dx(), other.dy()) {}
        // velocity2(float dx, float dy) : velocity_(dx, dy) {}
        velocity2(const velocity2& other) : dx(other.dx), dy(other.dy) {}
        velocity2(float dx, float dy) : dx(dx), dy(dy) {}

        float dx, dy;

        // float dx() {
        //     return velocity_.x;
        // }

        // void dx(float newDx) {
        //     velocity_.x = newDx;
        // }

        // float dy() {
        //     return velocity_.y;
        // }

        // void dy(float newDy) {
        //     velocity_.y = newDy;
        // }

    private:
        // glm::vec2 velocity_;
};

};

#endif



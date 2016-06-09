#ifndef Z_POS_2
#define Z_POS_2

#include "glm/glm.hpp"

namespace Z {

class pos2 {
    public:
        pos2();
        // pos2(pos2& other) : pos_(other.x(), other.y()) {}
        // pos2(float x, float y) : pos_(x, y) {}
        pos2(const pos2& other) : x(other.x), y(other.y) {}
        pos2(float x, float y) : x(x), y(y) {}

        float x, y;

        // float x() {
        //     return pos_.x;
        // }

        // void x(float newX) {
        //     pos_.x = newX;
        // }

        // float y() {
        //     return pos_.y;
        // }

        // void y(float newY) {
        //     pos_.y = newY;
        // }

    private:
        // glm::vec2 pos_;
};

};

#endif


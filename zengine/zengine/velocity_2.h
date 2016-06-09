#ifndef Z_VELOCITY_2
#define Z_VELOCITY_2

#include "glm/glm.hpp"

namespace Z {

class velocity2 {
    public:
        velocity2() : dx(0), dy(0) {}
        velocity2(const velocity2& other) : dx(other.dx), dy(other.dy) {}
        velocity2(float dx, float dy) : dx(dx), dy(dy) {}

        float dx, dy;

    private:
};

};

#endif



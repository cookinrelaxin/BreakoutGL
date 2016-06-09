#ifndef Z_POS_2
#define Z_POS_2

#include "glm/glm.hpp"

namespace Z {

class pos2 {
    public:
        pos2();
        pos2(const pos2& other) : x(other.x), y(other.y) {}
        pos2(float x, float y) : x(x), y(y) {}

        float x, y;

    private:
};

};

#endif


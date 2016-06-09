#ifndef Z_SIZE_2
#define Z_SIZE_2

#include "glm/glm.hpp"

namespace Z {

class size2 {
    public:
        size2() : width(0), height(0) {};
        size2(const size2& other) : width(other.width), height(other.height) {}
        size2(float width, float height) : width(width), height(height) {}

        float width, height;

    private:
};

};

#endif

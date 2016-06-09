#ifndef Z_VEC_2
#define Z_VEC_2

#include "glm/glm.hpp"

namespace Z {

class vec2 {
    public:
        vec2() : r(0), s(0) {}
        vec2(const vec2& other) : r(other.r), s(other.s) {}
        vec2(float r, float s) : r(r), s(s) {}

        float r, s;

    private:
        // glm::vec2 velocity_;
};

};

#endif




#ifndef Z_COLOR_4
#define Z_COLOR_4

#include "glm/glm.hpp"

namespace Z {

class color4 {
    public:
        color4() : r(1.0), g(1.0), b(1.0), a(1.0) {};
        color4(const color4& other)
            : r(other.r)
            , g(other.g)
            , b(other.b)
            , a(other.a) {}
        color4(float r, float g, float b, float a)
            : r(r)
            , g(g)
            , b(b)
            , a(a) {}

        float r,g,b,a;

    private:
};

};

#endif


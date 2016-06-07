#ifndef Z_EVENT
#define Z_EVENT

#include <ctime>

#include <GLFW/glfw3.h>

namespace Z {

    enum class MouseInput {
        LEFT_CLICK,
        RIGHT_CLICK
    };

    enum class KeyInput {
        SPACE = GLFW_KEY_SPACE,
        APOSTROPHE = GLFW_KEY_APOSTROPHE,
        COMMA = GLFW_KEY_COMMA,
        MINUS = GLFW_KEY_MINUS,
    };

class Event {
    public:
        Event() :t(std::time(nullptr)) {}
        ~Event() {}
        virtual time_t timestamp() const final {
            return t;
        }
    private:
        time_t t;
};

};

#endif


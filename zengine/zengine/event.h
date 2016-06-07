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
        PERIOD = GLFW_KEY_PERIOD,
        SLASH = GLFW_KEY_SLASH,
        ZERO = GLFW_KEY_0,
        ONE = GLFW_KEY_ONE,
        TWO = GLFW_KEY_TWO,
        THREE = GLFW_KEY_THREE,
        FOUR = GLFW_KEY_FOUR,
        FIVE = GLFW_KEY_FIVE,
        SIX = GLFW_KEY_SIX,
        SEVEN = GLFW_KEY_SEVEN,
        EIGHT = GLFW_KEY_EIGHT,
        NINE = GLFW_KEY_NINE,
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


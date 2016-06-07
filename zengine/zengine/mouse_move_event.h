#ifndef Z_MOUSE_MOVE_EVENT
#define Z_MOUSE_MOVE_EVENT

#include "event.h"

namespace Z {

class MouseMoveEvent : public Event {
    public:
        MouseMoveEvent(float dx, float dy) : delta_x(dx), delta_y(dy) {};
        const float dx() const {
            return delta_x;
        }
        const float dy() const {
            return delta_y;
        }
    private:
        const float delta_x;
        const float delta_y;
};

};

#endif



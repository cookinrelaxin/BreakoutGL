#ifndef Z_MOUSE_DOWN_EVENT
#define Z_MOUSE_DOWN_EVENT

#include "event.h"

namespace Z {

class MouseDownEvent : public Event {
    public:
        MouseDownEvent(MouseInput input) : b(input) {};
        MouseInput button() const {
            return b;
        }
    private:
        MouseInput b;
};

};

#endif

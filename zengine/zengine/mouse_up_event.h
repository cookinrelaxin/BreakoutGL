#ifndef Z_MOUSE_UP_EVENT
#define Z_MOUSE_UP_EVENT

#include "event.h"

namespace Z {

class MouseUpEvent : public Event {
    public:
        MouseUpEvent(MouseInput input) : b(input) {};
        MouseInput button() const {
            return b;
        }
    private:
        MouseInput b;
};

};

#endif


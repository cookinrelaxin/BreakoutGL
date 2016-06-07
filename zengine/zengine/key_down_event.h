#ifndef Z_KEY_DOWN_EVENT
#define Z_KEY_DOWN_EVENT

#include "event.h"

namespace Z {

class KeyDownEvent : public Event {
    public:
        KeyDownEvent(KeyInput key) : k(key) {};
        const KeyInput key() const {
            return k;
        }
    private:
        const KeyInput k;
};

};

#endif




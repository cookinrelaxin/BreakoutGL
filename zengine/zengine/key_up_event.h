#ifndef Z_KEY_UP_EVENT
#define Z_KEY_UP_EVENT

#include "event.h"

namespace Z {

class KeyUpEvent : public Event {
    public:
        KeyUpEvent(KeyInput key) : k(key) {};
        const KeyInput key() const {
            return k;
        }
    private:
        const KeyInput k;
};

};

#endif





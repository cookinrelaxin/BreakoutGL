#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "zengine/zengine.h"

class Breakout {
    public:
        static void init();
        static bool update(float dt);
        static void shutdown();

        static void mouseDown(Z::MouseDownEvent event);
        static void mouseUp(Z::MouseUpEvent event);
        static void mouseMove(Z::MouseMoveEvent event);

        static void keyDown(Z::KeyDownEvent event);
        static void keyUp(Z::KeyUpEvent event);
    private:

};

#endif

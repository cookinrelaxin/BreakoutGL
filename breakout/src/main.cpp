#include "breakout.h"
#include "zengine/zengine.h"

int main(int argc, char *argv[]) {
    // Game lifetime callbacks
    Z::Engine::registerInitCallback(&Breakout::init);
    Z::Engine::registerUpdateCallback(&Breakout::update);
    Z::Engine::registerShutdownCallback(&Breakout::shutdown);

    // Mouse event callbacks
    Z::Engine::registerMouseDownCallback(&Breakout::mouseDown);
    Z::Engine::registerMouseUpCallback(&Breakout::mouseUp);
    Z::Engine::registerMouseMoveCallback(&Breakout::mouseMove);

    // Keyboard event callbacks
    Z::Engine::registerKeyDownCallback(&Breakout::keyDown);
    Z::Engine::registerKeyUp(&Breakout::keyUp);

    // Initialize game engine
    Z::Engine::init();
}

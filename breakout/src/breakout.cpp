#include <iostream>

#include "zengine/zengine.h"
// #include "zengine/zengine.h"

#include "breakout.h"

const unsigned int SCREEN_WIDTH(1600), SCREEN_HEIGHT(1200);

void Breakout::init() {
    std::cout << "init" << std::endl;
}

bool Breakout::update(float dt) {
    std::cout << "update" << std::endl;
    return true;
}

void Breakout::shutdown() {
    std::cout << "shutdown" << std::endl;
}

void Breakout::mouseDown(Z::MouseDownEvent event) {
    std::cout << "mouseDown" << std::endl;
}

void Breakout::mouseUp(Z::MouseUpEvent event) {
    std::cout << "mouseUp" << std::endl;
}

void Breakout::mouseMove(Z::MouseMoveEvent event) {
    std::cout << "mouseMove dx: " << event.dx() << std::endl;
    std::cout << "mouseMove dy: " << event.dy() << std::endl;
}

void Breakout::keyDown(Z::KeyDownEvent event) {
    std::cout << "keyDown" << std::endl;
}

void Breakout::keyUp(Z::KeyUpEvent event) {
    std::cout << "keyUp" << std::endl;
}

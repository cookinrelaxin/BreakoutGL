#ifndef Z_ENGINE_H
#define Z_ENGINE_H

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <functional>

#include "mouse_down_event.h"
#include "mouse_up_event.h"
#include "mouse_move_event.h"

#include "key_down_event.h"
#include "key_up_event.h"


namespace Z {

class Engine {
    public:
        // Game lifetime callbacks
        static void registerInitCallback(std::function<void(void)> callback);
        static void registerUpdateCallback(std::function<bool(float)> callback);
        static void registerShutdownCallback(std::function<void(void)> callback);

        // Mouse event callbacks
        static void registerMouseDownCallback(
                std::function<void(MouseDownEvent)> callback);
        static void registerMouseUpCallback(
                std::function<void(MouseUpEvent)> callback);
        static void registerMouseMoveCallback(
                std::function<void(MouseMoveEvent)> callback);

        // Keyboard event callbacks
        static void registerKeyDownCallback(std::function<void(KeyDownEvent)> callback);
        static void registerKeyUp(std::function<void(KeyUpEvent)> callback);

        // Initialize game engine
        static void init();

        static void screenWidth(unsigned int width) {
            sWidth = width;
        };
        static void screenHeight(unsigned int height) {
            sHeight = height;
        };
        static unsigned int screenWidth() {
            return sWidth;
        };
        static unsigned int screenHeight() {
            return sHeight;
        };
    private:
        static void configureGLEW();
        static void configureGL();
        static void configureWindow(GLFWwindow* window);
        static GLFWwindow* createWindow();
        static void cleanup();

        static std::function<void(void)> initCallback;
        static std::function<bool(float)> updateCallback;
        static std::function<void(void)> shutdownCallback;
        static std::function<void(MouseDownEvent)> mouseDownCallback;
        static std::function<void(MouseUpEvent)> mouseUpCallback;
        static std::function<void(MouseMoveEvent)> mouseMoveCallback;
        static std::function<void(KeyDownEvent)> keyDownCallback;
        static std::function<void(KeyUpEvent)> keyUpCallback;

        static unsigned int sWidth;
        static unsigned int sHeight;
};

};

#endif

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

#include "scene_node.h"

#include <irrklang/irrKlang.h>

#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"

namespace Z {

class Engine {
    public:
        // Game lifetime callbacks
        static void registerInitCallback(std::function<SceneNode*(void)> callback);
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

    private:
        static void configureGLEW();
        static void configureGL(int width, int height);
        static void configureWindow(GLFWwindow* window);
        static GLFWwindow* createWindow(int width, int height);
        static void cleanup();
        static void keyHandler(GLFWwindow* window,
                              int key,
                              int scancode,
                              int action,
                              int mode);

        static std::function<SceneNode*(void)> initCallback;
        static std::function<bool(float)> updateCallback;
        static std::function<void(void)> shutdownCallback;
        static std::function<void(MouseDownEvent)> mouseDownCallback;
        static std::function<void(MouseUpEvent)> mouseUpCallback;
        static std::function<void(MouseMoveEvent)> mouseMoveCallback;
        static std::function<void(KeyDownEvent)> keyDownCallback;
        static std::function<void(KeyUpEvent)> keyUpCallback;

        static SceneNode* scene;
        static SpriteRenderer* renderer;
        static ParticleGenerator* particles;
        static PostProcessor* effects;
        static TextRenderer* text;
	static irrklang::ISoundEngine* soundEngine;
};

};

#endif

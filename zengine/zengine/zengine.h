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


namespace Z {

class Engine {
    public:

        /*      |~~~~~~~~~~~~~~~~~~~~~~~~~~|
         *      | Game lifecycle callbacks |
         *      |~~~~~~~~~~~~~~~~~~~~~~~~~~|
         */

        /*  The init callback is called by the engine before the engine fully starts up.
         *  Within the callback, the game should setup everything necessary (such as adding nodes to a scene) before the main update loop begins. The callback must return a SceneNode object, which will the be root of the game's node tree. 
         */
        static void registerInitCallback(std::function<SceneNode(SceneNode)> callback);

        /* The update callback is called by the engine every frame and represents the main loop of the game. 
         * The function must return a new SceneNode which represents changes to the state of the game. The returned SceneNode will become the game's new root for the duration of the next frame. The renderer draws everything within that node, and nothing else.
         * The idea is for there to be a Single Source of State for the entire game, which is that SceneNode object. The object itself is immutable. Because of this, it is impossible to mutate the state of the game from anywhere in the client code. The only way to update the game state is to return a new SceneNode object in this method.
         *
         */
        static void registerUpdateCallback(std::function<SceneNode(SceneNode)> callback);
        /*
         *
         */
        static void registerShutdownCallback(std::function<void(void)> callback);

        /*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    EDIT: THERE ARE NO SEPARATE INPUT EVENT CALLBACKS. EVERYTHING PASSES THROUGH THE UPDATE FUNCTION.
         * /

        /*      |~~~~~~~~~~~~~~~~~~~~~~~~~|
         *      |  Mouse event callbacks  |
         *      |~~~~~~~~~~~~~~~~~~~~~~~~~|
         */

        /*
         *
         */
        static void registerMouseDownCallback(
                std::function<void(MouseDownEvent)> callback);
        /*
         *
         */
        static void registerMouseUpCallback(
                std::function<void(MouseUpEvent)> callback);
        /*
         *
         */
        static void registerMouseMoveCallback(
                std::function<void(MouseMoveEvent)> callback);

        /*      |~~~~~~~~~~~~~~~~~~~~~~~~~~~|
         *      |  Keyboard event callbacks |
         *      |~~~~~~~~~~~~~~~~~~~~~~~~~~~|
         */

        static void registerKeyDownCallback(std::function<void(KeyDownEvent)> callback);
        /*
         *
         */
        static void registerKeyUp(std::function<void(KeyUpEvent)> callback);

        // Initialize game engine
        /*
         *
         */
        static void init();

        /*
         *
         */
        static void screenWidth(unsigned int width) {
            sWidth = width;
        };
        /*
         *
         */
        static void screenHeight(unsigned int height) {
            sHeight = height;
        };
        /*
         *
         */
        static unsigned int screenWidth() {
            return sWidth;
        };
        /*
         *
         */
        static unsigned int screenHeight() {
            return sHeight;
        };
        /*
         *
         */
    private:
        /*
         *
         */
        static void configureGLEW();
        /*
         *
         */
        static void configureGL();
        /*
         *
         */
        static void configureWindow(GLFWwindow* window);
        /*
         *
         */
        static GLFWwindow* createWindow();
        /*
         *
         */
        static void cleanup();

        static std::function<void(void)> initCallback;
        /*
         *
         */
        static std::function<bool(float)> updateCallback;
        /*
         *
         */
        static std::function<void(void)> shutdownCallback;
        /*
         *
         */
        static std::function<void(MouseDownEvent)> mouseDownCallback;
        /*
         *
         */
        static std::function<void(MouseUpEvent)> mouseUpCallback;
        /*
         *
         */
        static std::function<void(MouseMoveEvent)> mouseMoveCallback;
        /*
         *
         */
        static std::function<void(KeyDownEvent)> keyDownCallback;
        /*
         *
         */
        static std::function<void(KeyUpEvent)> keyUpCallback;
        /*
         *
         */
        static unsigned int sWidth;
        /*
         *
         */
        static unsigned int sHeight;
};

};

#endif

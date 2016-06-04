#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <GLFW/glfw3.h>

class EventHandler {
    public:
        static void handle_keys(GLFWwindow* window, int key, int scancode, int action, int mode);

        static GLboolean Keys[1024];
        static GLboolean KeysProcessed[1024];
};

GLboolean EventHandler::Keys[1024];
GLboolean EventHandler::KeysProcessed[1024];

//~~~~~~~~~~~~~~~~~

void EventHandler::handle_keys(GLFWwindow* window, int key, int scancode, int action, int mode) {
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            Keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
            Keys[key] = GL_FALSE;
            KeysProcessed[key] = GL_FALSE;
    }

}

#endif

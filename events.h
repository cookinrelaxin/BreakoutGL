#ifndef EVENTS_H
#define EVENTS_H

#include <GLFW/glfw3.h>

#include "game.h"

void handle_keys(GLFWwindow* window, int key, int scancode, int action, int mode);

//~~~~~~~~~~~~~~~~~

void handle_keys(Game &Breakout, GLFWwindow* window, int key, int scancode, int action, int mode) {
    // std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            Breakout.Keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
            Breakout.Keys[key] = GL_FALSE;
    }

}

#endif

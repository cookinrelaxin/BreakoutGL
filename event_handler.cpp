#include "event_handler.h"
#include <iostream>

// GLboolean EventHandler::Keys[1024];
// GLboolean EventHandler::KeysProcessed[1024];

std::unordered_set<int> EventHandler::KeysDown; 
std::unordered_set<int> EventHandler::KeysDownProcessed; 
std::unordered_set<int> EventHandler::KeysUp; 

void EventHandler::handle_keys(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            KeysDown.insert(key);
        }
        else if (action == GLFW_RELEASE) {
            KeysDown.erase(key);
            KeysDownProcessed.erase(key);

            KeysUp.insert(key);
        }
    }

}

#ifndef EVENTS_H
#define EVENTS_H

#include <GLFW/glfw3.h>

#include <unordered_set>

class EventHandler {
    public:
        static void handle_keys(GLFWwindow* window, int key, int scancode, int action, int mode);

        static std::unordered_set<int> KeysDown; 
        static std::unordered_set<int> KeysDownProcessed; 
        static std::unordered_set<int> KeysUp; 
};

#endif

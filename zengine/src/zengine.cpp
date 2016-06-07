
// #include "resource_manager.h"

//STL
#include <iostream>

// #include "events.h"
#include "zengine.h"
#include "no_callback_error.h"

namespace Z {

std::function<void(void)>  Engine::initCallback;
std::function<bool(float)> Engine::updateCallback;
std::function<void(void)>  Engine::shutdownCallback;
std::function<void(MouseDownEvent)>  Engine::mouseDownCallback;
std::function<void(MouseUpEvent)>  Engine::mouseUpCallback;
std::function<void(MouseMoveEvent)>  Engine::mouseMoveCallback;
std::function<void(KeyDownEvent)>  Engine::keyDownCallback;
std::function<void(KeyUpEvent)>  Engine::keyUpCallback;

unsigned int Engine::sWidth;
unsigned int Engine::sHeight;

void Engine::registerInitCallback(std::function<void(void)> callback) {
    initCallback = callback;
}

void Engine::registerUpdateCallback(std::function<bool(float)> callback) {
    updateCallback = callback;
}

void Engine::registerShutdownCallback(std::function<void(void)> callback) {
    shutdownCallback = callback;
}

void Engine::registerMouseDownCallback(std::function<void(MouseDownEvent)> callback) {
    mouseDownCallback = callback;
}

void Engine::registerMouseUpCallback(std::function<void(MouseUpEvent)> callback) {
    mouseUpCallback = callback;
}

void Engine::registerMouseMoveCallback(std::function<void(MouseMoveEvent)> callback) {
    mouseMoveCallback = callback;
}

void Engine::registerKeyDownCallback(std::function<void(KeyDownEvent)> callback) {
    keyDownCallback = callback;
}

void Engine::registerKeyUp(std::function<void(KeyUpEvent)> callback) {
    keyUpCallback = callback;
}

void Engine::init() {
    if (!initCallback)
        throw NoCallbackError("initCallback");
    if (!updateCallback)
        throw NoCallbackError("updateCallback");
    if (!shutdownCallback)
        throw NoCallbackError("shutdownCallback");
    if (!mouseDownCallback)
        throw NoCallbackError("mouseDownCallback");
    if (!mouseUpCallback)
        throw NoCallbackError("mouseUpCallback");
    if (!mouseMoveCallback)
        throw NoCallbackError("mouseMoveCallback");
    if (!keyDownCallback)
        throw NoCallbackError("keyDownCallback");
    if (!keyUpCallback)
        throw NoCallbackError("keyUpCallback");

    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    GLFWwindow* window = createWindow();
    // configureWindow(window);

    configureGLEW();
    configureGL();

}

GLFWwindow* Engine::createWindow() {
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(sWidth, sHeight, "Breakout", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    return window;
}

void Engine::configureGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    glGetError();
}

void Engine::cleanup() {
    // ResourceManager::Clear();
    glfwTerminate();
}

void Engine::configureGL() {
    glViewport(0, 0, sWidth, sHeight);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// void configureWindow(GLFWwindow* window) {
//     glfwMakeContextCurrent(window);
//     glfwSetKeyCallback(window,
//             [](GLFWwindow* window, int key, int scancode, int action, int mode) {
//                 handle_keys(Breakout, window, key, scancode, action, mode);
//             }
//     );
// }

// 
// int main(int argc, char *argv[]) {
//     std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
//     GLFWwindow* window = create_window();
//     configureWindow(window);
// 
//     configure_glew();
//     configureGL();
// 
//     Breakout.Init();
// 
//     return [&window]() {
//         GLfloat currentTime(glfwGetTime());
//         ExitStatus status = loop(window, currentTime, currentTime);
//         cleanup();
//         return status;
//     }();
// }
// 
// ExitStatus loop(GLFWwindow * window, GLfloat currentFrame, GLfloat lastFrame) {
//    return (glfwWindowShouldClose(window)
//            ? ExitOK
//            : [=]() {
//                  GLfloat deltaTime = currentFrame - lastFrame;
// 
//                  glfwPollEvents();
// 
//                  Breakout.ProcessInput(deltaTime);
// 
//                  Breakout.Update(deltaTime);
// 
//                  // glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
//                  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//                  glClear(GL_COLOR_BUFFER_BIT);
//                  Breakout.Render();
// 
//                  glfwSwapBuffers(window);
// 
//                  return loop(window, glfwGetTime(), currentFrame);
//              }());
// }
};

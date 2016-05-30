// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"

//STL
#include <iostream>

//
#include "events.h"

enum ExitStatus {
    ExitOK,
    ExitError
};

ExitStatus loop(GLFWwindow* window, GLfloat currentFrame, GLfloat lastFrame);

// Window dimensions
const GLuint SCREEN_WIDTH(1600), SCREEN_HEIGHT(1200);

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

GLFWwindow* create_window() {
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    return window;
}

void configure_glew() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
    glGetError();
}

void cleanup() {
    ResourceManager::Clear();
    glfwTerminate();
}

void configureGL() {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void configureWindow(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window,
            [](GLFWwindow* window, int key, int scancode, int action, int mode) {
                handle_keys(Breakout, window, key, scancode, action, mode);
            }
    );
}

int main(int argc, char *argv[]) {
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    GLFWwindow* window = create_window();
    configureWindow(window);

    configure_glew();
    configureGL();

    Breakout.Init();

    return [&window]() {
        GLfloat currentTime(glfwGetTime());
        ExitStatus status = loop(window, currentTime, currentTime);
        cleanup();
        return status;
    }();
}

ExitStatus loop(GLFWwindow * window, GLfloat currentFrame, GLfloat lastFrame) {
   return (glfwWindowShouldClose(window)
           ? ExitOK
           : [=]() {
                 GLfloat deltaTime = currentFrame - lastFrame;

                 glfwPollEvents();

                 Breakout.ProcessInput(deltaTime);

                 Breakout.Update(deltaTime);

                 // glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
                 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                 glClear(GL_COLOR_BUFFER_BIT);
                 Breakout.Render();

                 glfwSwapBuffers(window);

                 return loop(window, glfwGetTime(), currentFrame);
             }());
}

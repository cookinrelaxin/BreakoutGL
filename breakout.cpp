// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// #include "resource_manager.h"

//STL
#include <iostream>

//
// #include "events.h"

#include "shell.h"

#include "zscene_node.h"

#include <thread>
#include <chrono>

enum ExitStatus {
    ExitOK,
    ExitError
};

ExitStatus loop(Shell shell, GLFWwindow* window, GLfloat currentFrame, GLfloat lastFrame);

// Window dimensions
const GLuint SCREEN_WIDTH(1600), SCREEN_HEIGHT(1200);

GLFWwindow* create_window() {
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
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
    // ResourceManager::Clear();
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
    // glfwSetKeyCallback(window,
    //         [](GLFWwindow* window, int key, int scancode, int action, int mode) {
    //             handle_keys(Breakout, window, key, scancode, action, mode);
    //         }
    // );
}

int main(int argc, char *argv[]) {
    GLFWwindow* window = create_window();
    configureWindow(window);

    configure_glew();
    configureGL();

    Shell shell(argc, argv);
    ZSceneNode* scene = shell.Init();

    GLfloat currentFrame = glfwGetTime();
    GLfloat lastFrame = currentFrame;

    while (true) {
        currentFrame = glfwGetTime();
        GLfloat deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        shell.Update(deltaTime);
        shell.Poll();

        glm::vec4 clear = scene->get_background_color();
        glClearColor(clear.x, clear.y, clear.z, clear.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
    return 0;
}

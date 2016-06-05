// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "event_handler.h"

#include "shell.h"

#include "zscene_node.h"

#include "resource_manager.h"
#include "sprite_renderer.h"
#include "shader.h"
#include "texture.h"
#include "post_processor.h"

#include <thread>
#include <chrono>

void process_input(Shell& shell);

GLFWwindow* create_window(int width, int height, std::string title) {
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
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

void configureGL(int width, int height) {
    glViewport(0, 0, width, height);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    // glEnable(GL_POLYGON_SMOOTH);
    // glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void setViewport(GLFWwindow* window, std::string title, int width, int height) {
    glfwSetWindowSize(window, width, height);
    glfwSetWindowTitle(window, title.c_str());
    glViewport(0, 0, width, height);
}

void configureWindow(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    auto callback = [](GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mode) {
            EventHandler::handle_keys(window, key, scancode, action, mode);
            return;
    };
                      
    // glfwSetKeyCallback(window,handle_keys);
    glfwSetKeyCallback(window,callback);
}

int main() {
    int DEFAULT_WIDTH(800), DEFAULT_HEIGHT(600);

    GLFWwindow* window = create_window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "booga");
    glfwHideWindow(window);
    configureWindow(window);
    configure_glew();
    assert(glGetError() == GL_NO_ERROR);
    configureGL(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    assert(glGetError() == GL_NO_ERROR);

    Shell shell;
    ZSceneNode* scene = shell.Init();
    int width = scene->get_size().x;
    int height = scene->get_size().y;
    std::string title = scene->get_name();
    assert(glGetError() == GL_NO_ERROR);

    setViewport(window, title, width, height);
    glfwShowWindow(window);
    assert(glGetError() == GL_NO_ERROR);

    ResourceManager::LoadShader("./shaders/sprite.vs",
                               "./shaders/sprite.fs",
                               nullptr,
                               "sprite_shader");
    ResourceManager::LoadShader("./shaders/post_process.vs",
                               "./shaders/post_process.fs",
                               nullptr,
                               "postprocessing_shader");
    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<GLfloat>(width),
                                      static_cast<GLfloat>(height),
                                      0.0f,
                                      -1.0f,
                                      1.0f);
    ResourceManager::GetShader("sprite_shader").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite_shader").SetMatrix4("projection", projection);
    Shader sprite_shader = ResourceManager::GetShader("sprite_shader");
    SpriteRenderer renderer(sprite_shader);
    PostProcessor  postProcessor(
            ResourceManager::GetShader("postprocessing_shader"),
            width,
            height);
    assert(glGetError() == GL_NO_ERROR);

    GLfloat currentFrame = glfwGetTime();
    GLfloat lastFrame = currentFrame;
    assert(glGetError() == GL_NO_ERROR);

    while (true) {
        if (glfwWindowShouldClose(window)) break;
        currentFrame = glfwGetTime();
        GLfloat deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        process_input(shell);
        shell.Update(deltaTime);
        shell.Poll();

        glm::vec4 clear = scene->get_background_color();
        glClearColor(clear.x, clear.y, clear.z, clear.w);
        glClear(GL_COLOR_BUFFER_BIT);

        postProcessor.BeginRender();
        for (ZNode* node : scene->get_children()) {
            node->draw(renderer);
        }
        postProcessor.EndRender();
        postProcessor.Render(currentFrame);
        glfwSwapBuffers(window);
    }
    shell.UninitializeV8();
    cleanup();
    return 0;
}

void process_input(Shell& shell) {
    auto& keysDown = EventHandler::KeysDown;
    auto& keysDownProcessed = EventHandler::KeysDownProcessed;
    auto& keysUp = EventHandler::KeysUp;
    for (int key : keysDown) {
        if (keysDownProcessed.count(key) == 0) {
            shell.KeyDown(key);
            EventHandler::KeysDownProcessed.insert(key);
        }
    }
    for (int key : keysUp) {
        shell.KeyUp(key);
    }
    keysUp.clear();
}

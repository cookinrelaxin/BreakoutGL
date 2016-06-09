#include "resource_manager.h"

//STL
#include <iostream>

// #include "events.h"
#include "zengine.h"
#include "no_callback_error.h"

#include <glm/gtc/matrix_transform.hpp>

#include "event.h"
#include "key_down_event.h"
#include "key_up_event.h"

namespace Z {

std::function<SceneNode*(void)>  Engine::initCallback;
std::function<bool(float)> Engine::updateCallback;
std::function<void(void)>  Engine::shutdownCallback;
std::function<void(MouseDownEvent)>  Engine::mouseDownCallback;
std::function<void(MouseUpEvent)>  Engine::mouseUpCallback;
std::function<void(MouseMoveEvent)>  Engine::mouseMoveCallback;
std::function<void(KeyDownEvent)>  Engine::keyDownCallback;
std::function<void(KeyUpEvent)>  Engine::keyUpCallback;

SceneNode* Engine::scene;
SpriteRenderer* Engine::renderer;
ParticleGenerator* Engine::particles;
PostProcessor* Engine::effects;
TextRenderer* Engine::text;
irrklang::ISoundEngine* Engine::soundEngine;

void Engine::registerInitCallback(std::function<SceneNode*(void)> callback) {
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
    int defaultWidth = 1024;
    int defaultHeight = 768;
    GLFWwindow* window = createWindow(defaultWidth, defaultHeight);
    configureGLEW();
    assert(glGetError() == GL_NO_ERROR);

    scene = initCallback();
    int width = scene->size.width;
    int height = scene->size.height;
    assert(glGetError() == GL_NO_ERROR);

    configureGL(width, height);

    assert(glGetError() == GL_NO_ERROR);

    ResourceManager::LoadShader("./assets/shaders/sprite.vs",
                                "./assets/shaders/sprite.fs",
                                nullptr,
                                "sprite");

    ResourceManager::LoadShader("./assets/shaders/particle.vs",
                                "./assets/shaders/particle.fs",
                                nullptr,
                                "particle");

    ResourceManager::LoadShader("./assets/shaders/post_process.vs",
                                "./assets/shaders/post_process.fs",
                                nullptr,
                                "post_process");
    assert(glGetError() == GL_NO_ERROR);
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<GLfloat>(width), 
                                      static_cast<GLfloat>(height),
                                      0.0f,
                                      -1.0f,
                                      1.0f);

    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);	
    assert(glGetError() == GL_NO_ERROR);

    // Set render-specific controls
    Shader sh = ResourceManager::GetShader("sprite");
    renderer = new SpriteRenderer(sh);
    assert(glGetError() == GL_NO_ERROR);
    particles = new ParticleGenerator(ResourceManager::GetShader("particle"),
                                      ResourceManager::GetTexture("particle"),
                                      500);
    assert(glGetError() == GL_NO_ERROR);
    effects = new PostProcessor(ResourceManager::GetShader("post_process"),
                                width,
                                height
    );
    assert(glGetError() == GL_NO_ERROR);
    text = new TextRenderer(width, height);
    assert(glGetError() == GL_NO_ERROR);
    text->Load("./assets/fonts/OCRAEXT.TTF", 24);
    assert(glGetError() == GL_NO_ERROR);

    soundEngine = irrklang::createIrrKlangDevice();
    soundEngine->loadPlugins("ikpMP3.dylib");
    assert(glGetError() == GL_NO_ERROR);

    float currentFrame, lastFrame;
    currentFrame = lastFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;

 
    glfwShowWindow(window);
    while (updateCallback(deltaTime)) {
        glfwPollEvents();
        deltaTime = currentFrame - lastFrame;
        glClearColor(scene->backgroundColor.x,
                     scene->backgroundColor.y,
                     scene->backgroundColor.z,
                     scene->backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
	effects->BeginRender();
	scene->draw(renderer);
	effects->EndRender();
	effects->Render(currentFrame);
        
        glfwSwapBuffers(window);
    };
    shutdownCallback();
    cleanup();

    return;
}

GLFWwindow* Engine::createWindow(int width, int height) {
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(width, height, "Breakout", nullptr, nullptr);
    configureWindow(window);
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

void Engine::configureGL(int width, int height) {
    glViewport(0, 0, width, height);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Engine::configureWindow(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, &Engine::keyHandler);
}

void Engine::keyHandler(GLFWwindow* window,
                        int key,
                        int scancode,
                        int action,
                        int mode) {
    switch (action) {
        case GLFW_PRESS: {
            KeyInput input(static_cast<KeyInput>(key));
            KeyDownEvent event(input);
            keyDownCallback(event);
            break;
        }

        case GLFW_RELEASE: {
            KeyInput input(static_cast<KeyInput>(key));
            KeyUpEvent event(input);
            keyUpCallback(event);
            break;
        }
    }
}

};

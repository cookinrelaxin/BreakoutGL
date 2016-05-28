#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <iostream>

SpriteRenderer *Renderer;

Game::Game(GLuint width, GLuint height)
    : State(GAME_MENU)
    , Keys()
    , Width(width)
    , Height(height) {

}

Game::~Game() {
    delete Renderer;
}

void Game::Init() {
    // Load shaders
    ResourceManager::LoadShader("shaders/sprite.vs",
                                "shaders/sprite.fs",
                                nullptr,
                                "sprite");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<GLfloat>(this->Width), 
                                      static_cast<GLfloat>(this->Height),
                                      0.0f,
                                      -1.0f,
                                      1.0f);

    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // Load textures

    ResourceManager::LoadTexture("potato.png",
                                 GL_TRUE,
                                 "face");

    Texture2D tex = ResourceManager::GetTexture("face");



    // Set render-specific controls
    Shader sh = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(sh);
}

void Game::Update(GLfloat dt) {

}

void Game::ProcessInput(GLfloat dt) {

}

void Game::Render() {
    Texture2D tex = ResourceManager::GetTexture("face");

    Renderer->DrawSprite(tex,
                         glm::vec2(200, 200),
                         glm::vec2(64, 64),
                         0.0f,
                         glm::vec3(0.0f, 1.0f, 0.0f));
}

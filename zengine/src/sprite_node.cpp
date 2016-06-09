#include "sprite_node.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

#include <iostream>

namespace Z {

SpriteNode::SpriteNode() {
    size = size2(10.0, 10.0);
    position = pos2(0.0, 0.0);
    rotation = 0.0;
    color = color4(1.0, 0.0, 0.0, 1.0);
    velocity = velocity2(0.0, 0.0);
    texturePath = "./assets/textures/white_background.png";
    this->texture = ResourceManager::LoadTexture(
            texturePath.c_str(),
            true,
            texturePath);
}
SpriteNode::SpriteNode(size2       size,
                       pos2        position,
                       std::string texturePath,
                       color4      color,
                       float       rotation,
                       velocity2   velocity) 
    : texturePath(texturePath)
    , rotation(0.0)
    , color(color) {
    this->position = position;
    this->size = size;
    this->velocity = velocity;

    assert(glGetError() == GL_NO_ERROR);
    std::string extension = texturePath.substr(texturePath.find_last_of(".") + 1);
    auto use_alpha = [extension]() {
        if (extension == "png" or extension == "tga")
            return GL_TRUE;
        return GL_FALSE;
    };
    this->texture = ResourceManager::LoadTexture(
            texturePath.c_str(),
            use_alpha(),
            texturePath);
}

SpriteNode::~SpriteNode() {
}

void SpriteNode::setTexture(std::string texturePath) {
    this->texturePath = texturePath;
    assert(glGetError() == GL_NO_ERROR);
    std::string extension = texturePath.substr(texturePath.find_last_of(".") + 1);
    auto use_alpha = [extension]() {
        if (extension == "png" or extension == "tga")
            return GL_TRUE;
        return GL_FALSE;
    };
    this->texture = ResourceManager::LoadTexture(
            texturePath.c_str(),
            use_alpha(),
            texturePath);
}

void SpriteNode::draw(SpriteRenderer *renderer) {
    renderer->DrawSprite(texture,
                        glm::vec2(position.x, position.y),
                        glm::vec2(size.width, size.height),
                        rotation,
                        glm::vec3(color.r, color.g, color.b));
}

};

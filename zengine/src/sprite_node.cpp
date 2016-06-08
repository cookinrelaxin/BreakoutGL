#include "sprite_node.h"

namespace Z {

SpriteNode::SpriteNode() : color_(1.0, 1.0, 1.0, 1.0), texturePath_("./wooga.png") {}
SpriteNode::~SpriteNode() {
}

glm::vec4 SpriteNode::color() {
    return color_;
}

void SpriteNode::color(glm::vec4 newColor) {
    color_ = newColor;
}

std::string texture() {
    return texturePath_;
}

void texture(std::string texturePath) {
    texturePath_ = texturePath;
}

};

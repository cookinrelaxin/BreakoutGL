#include "scene_node.h"
#include <iostream>

namespace Z {

SceneNode::SceneNode() : backgroundColor(1.0,1.0,1.0,1.0) {}

SceneNode::~SceneNode() {
}

void SceneNode::draw(SpriteRenderer* spriteRenderer, TextRenderer* textRenderer) {
    for (Node* node : children_)
        node->draw(spriteRenderer, textRenderer);
}

};

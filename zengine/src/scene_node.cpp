#include "scene_node.h"
#include <iostream>

namespace Z {

SceneNode::SceneNode() : backgroundColor(1.0,1.0,1.0,1.0) {}

SceneNode::~SceneNode() {
}

void SceneNode::draw(SpriteRenderer* renderer) {
    // std::cout << "draw scene node" << std::endl;
    // std::cout << "children_.size(): " << children_.size() << std::endl;
    for (Node* node : children_)
        node->draw(renderer);
}

};

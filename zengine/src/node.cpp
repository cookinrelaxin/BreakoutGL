#include "node.h"

#include "sprite_renderer.h"

#include <iostream>

namespace Z {

Node::~Node() {
}

void Node::addChild(Node* child) {
    children_.insert(child);
}

// void removeChild(Node* child) {
// }

void Node::draw(SpriteRenderer* renderer) {
    // std::cout << "draw node children" << std::endl;
    for (Node* node : children_)
        node->draw(renderer);
}

};

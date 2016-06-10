#include "node.h"

#include "sprite_renderer.h"

#include <iostream>

namespace Z {

Node::~Node() {
}

void Node::addChild(Node* child) {
    children_.insert(child);
    child->parent = this;
}

void Node::removeChild(std::string name) {
    for (auto it  = children_.begin(); it != children_.end(); ++it) {
        if ((*it)->name == name)
            children_.erase(it);
    }
}

void Node::draw(SpriteRenderer* spriteRenderer, TextRenderer* textRenderer) {
    // std::cout << "draw node children" << std::endl;
    for (Node* node : children_)
        node->draw(spriteRenderer, textRenderer);
}

};

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
    auto toErase = children_.end();
    for (auto it = children_.begin(); it != children_.end(); ++it) {
        if ((*it)->name == name) {
            toErase = it;
            break;
        }
    }
    if (toErase != children_.end())
        children_.erase(toErase);
}

void Node::draw(SpriteRenderer* spriteRenderer, TextRenderer* textRenderer) {
    // std::cout << "draw node children" << std::endl;
    for (Node* node : children_)
        node->draw(spriteRenderer, textRenderer);
}

void Node::hide() {
    for (Node* node : children_)
        node->hide();
};

void Node::show() {
    for (Node* node : children_)
        node->show();
};

};

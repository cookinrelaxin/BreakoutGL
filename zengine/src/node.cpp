#include "node.h"

namespace Z {

Node::~Node() {
}

glm::vec2 Node::size() {
    return size_;
}

void Node::size(glm::vec2 newSize) {
    size_ = newSize;
}

glm::vec2 Node::position() {
    return position_;
}

void Node::position(glm::vec2 newPosition) {
    position_ = newPosition;
}

int Node::zPosition() {
    return zPosition_;
}

void Node::zPosition(int newZPosition) {
    zPosition_ = newZPosition;
}

void Node::addChild(Node* child) {
    children_.push_back(child);
}

// void removeChild(Node* child) {
// }

void Node::draw(SpriteRenderer& renderer) const {
    for (Node* node : children_)
        node->draw(renderer);
}

};

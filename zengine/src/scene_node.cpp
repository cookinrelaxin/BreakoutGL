#include "scene_node.h"

namespace Z {

SceneNode::SceneNode() : backgroundColor_(1.0,1.0,1.0,1.0) {}

SceneNode::~SceneNode() {
}

glm::vec4 SceneNode::backgroundColor() {
    return backgroundColor_;
}

void SceneNode::backgroundColor(glm::vec4 newBackgroundColor) {
    backgroundColor_ = newBackgroundColor;
}

};

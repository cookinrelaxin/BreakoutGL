#include "zscene_node.h"
#include "shell.h"

#include "v8pp/class.hpp"

ZSceneNode::ZSceneNode() : background_color_(0,0,0,0) {
    v8pp::class_<glm::vec4>::reference_external(Shell::_isolate, &background_color_);
}

ZSceneNode::~ZSceneNode() {
    v8pp::class_<glm::vec4>::unreference_external(Shell::_isolate, &background_color_);
}

glm::vec4& ZSceneNode::get_background_color() {
    return background_color_;
}
void ZSceneNode::set_background_color(glm::vec4 new_background_color) {
    background_color_ = new_background_color;
}


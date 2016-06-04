#include "znode.h"
#include "shell.h"

#include "v8pp/class.hpp"

ZNode::ZNode() : pos_(0,0), size_(1024, 768), zpos_(0) {
    v8pp::class_<glm::vec2>::reference_external(Shell::_isolate, &pos_);
    v8pp::class_<glm::vec2>::reference_external(Shell::_isolate, &size_);
    // v8pp::class_<int>::reference_external(Shell::_isolate, &zpos_);
    // v8pp::class_<std::string>::reference_external(Shell::_isolate, &name_);
    // v8pp::class_<ZNode*>::reference_external(Shell::_isolate, &parent_);
}

ZNode::~ZNode() {
    v8pp::class_<glm::vec2>::unreference_external(Shell::_isolate, &pos_);
    v8pp::class_<glm::vec2>::unreference_external(Shell::_isolate, &size_);
    // v8pp::class_<int>::unreference_external(Shell::_isolate, &zpos_);
    // v8pp::class_<std::string>::unreference_external(Shell::_isolate, &name_);
    // v8pp::class_<ZNode*>::unreference_external(Shell::_isolate, &parent_);
}

glm::vec2& ZNode::get_position() {
    return pos_;
}

void ZNode::set_position(glm::vec2 new_pos) {
    pos_ = new_pos;
}

int& ZNode::get_zposition() {
    return zpos_;
}

void ZNode::set_zposition(int new_zpos) {
    zpos_ = new_zpos;
}

glm::vec2& ZNode::get_size() {
    return size_;
}

void ZNode::set_size(glm::vec2 new_size) {
    size_ = new_size;
}

void ZNode::add_child(ZNode* child) {
    // children_.insert(child->get_name(), child);
    children_.insert(child);
};
// 
// ZNode* ZNode::get_child(std::string child_name) {
//     return children_.find(child_name);
// }

// void removeChild(ZNode* child) {
//     children_.
// };
std::set<ZNode*, ZNode::NodeComparator> ZNode::get_children() {
    return children_;
};
ZNode* ZNode::get_parent() {
    return parent_;
};

std::string ZNode::get_name() {
    return name_;
}
void ZNode::set_name(std::string new_name) {
    name_ = new_name;
};

void ZNode::draw(SpriteRenderer& renderer) {
    return;
}

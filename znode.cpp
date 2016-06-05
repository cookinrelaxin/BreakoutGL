#include "znode.h"
#include "shell.h"
#include "collisions.h"

#include "v8pp/class.hpp"

#include <iostream>

ZNode::ZNode() : pos_(0,0), size_(1024, 768), zpos_(0) {
    v8::Isolate* isolate = Shell::_context->isolate();
    v8pp::class_<glm::vec2>::reference_external(isolate, &pos_);
    v8pp::class_<glm::vec2>::reference_external(isolate, &size_);

    // v8pp::class_<int>::reference_external(Shell::_isolate, &zpos_);
    // v8pp::class_<std::string>::reference_external(Shell::_isolate, &name_);
    // v8pp::class_<ZNode*>::reference_external(Shell::_isolate, &parent_);
}

ZNode::~ZNode() {
    v8::Isolate* isolate = Shell::_context->isolate();
    v8pp::class_<glm::vec2>::unreference_external(isolate, &pos_);
    v8pp::class_<glm::vec2>::unreference_external(isolate, &size_);

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

void ZNode::remove_child(ZNode* child) {
    // children_.insert(child->get_name(), child);
    children_.erase(child);
};
// 
// ZNode* ZNode::get_child(std::string child_name) {
//     return children_.find(child_name);
// }

// void removeChild(ZNode* child) {
//     children_.
// };
std::multiset<ZNode*, ZNode::NodeComparator> ZNode::get_children() {
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

void ZNode::doesCollideAABBAABB(v8::FunctionCallbackInfo<v8::Value> const& args) {
    v8::HandleScope handle_scope(args.GetIsolate());
    // ZNode& other = args[0];
    ZNode *other = &v8pp::from_v8<ZNode>(args.GetIsolate(), args[0]);
    if (Collisions::CheckCollisionAABBAABB(this, other))
        args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), true));
    else    
        args.GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), false));
}

void ZNode::getCollision(v8::FunctionCallbackInfo<v8::Value> const& args) {
    v8::HandleScope handle_scope(args.GetIsolate());
    ZNode *other = &v8pp::from_v8<ZNode>(args.GetIsolate(), args[0]);
    Collision collision = Collisions::CheckCollision(this, other);

    std::string directionString = [](Direction d) {
        switch (d) {
            case Direction::UP :
                return "UP";
            case Direction::RIGHT :
                return "RIGHT";
            case Direction::DOWN :
                return "DOWN";
            case Direction::LEFT :
                return "LEFT";
        }
    }(std::get<1>(collision));

    v8::Handle<v8::Object> jsCollision = v8pp::class_<ZCollision>::create_object(
            args.GetIsolate(),
            std::get<0>(collision),
            directionString,
            std::get<2>(collision));

    args.GetReturnValue().Set(jsCollision);
}

v8pp::class_<ZNode> ZNode::create(v8::Isolate* isolate) {
    v8pp::class_<ZNode> znode_class(isolate);
    znode_class
        .ctor<>()
        .set("position", v8pp::property(&ZNode::get_position, &ZNode::set_position))
        .set("zPosition", v8pp::property(&ZNode::get_zposition, &ZNode::set_zposition))
        .set("size", v8pp::property(&ZNode::get_size, &ZNode::set_size))
        .set("name", v8pp::property(&ZNode::get_name, &ZNode::set_name))
        .set("addChild", &ZNode::add_child)
        .set("removeChild", &ZNode::remove_child)
        // .set("getChild", &ZNode::get_child)
        .set("collides", &ZNode::doesCollideAABBAABB)
        .set("getCollision", &ZNode::getCollision)
        ;
    return znode_class;
}

#include <iostream>
#include <string>

#include "zsprite_node.h"
#include "shell.h"
#include "resource_manager.h"
#include "texture.h"

#include "v8pp/class.hpp"

ZSpriteNode::ZSpriteNode() {
    v8::Isolate* isolate = Shell::_context->isolate();
    v8pp::class_<glm::vec4>::reference_external(isolate, &color_);
}

ZSpriteNode::~ZSpriteNode() {
    v8::Isolate* isolate = Shell::_context->isolate();
    v8pp::class_<glm::vec4>::unreference_external(isolate, &color_);
}

glm::vec4& ZSpriteNode::get_color() {
    return color_;
}

void ZSpriteNode::set_color(glm::vec4 new_color) {
    color_ = new_color;
}

std::string ZSpriteNode::get_texture() {
    return texture_name_;
};
void ZSpriteNode::set_texture(std::string texture_path) {
    assert(glGetError() == GL_NO_ERROR);
    std::string extension = texture_path.substr(texture_path.find_last_of(".") + 1);
    auto use_alpha = [extension]() {
        if (extension == "png")
            return GL_TRUE;
        return GL_FALSE;
    };
    ResourceManager::LoadTexture(texture_path.c_str(),
                                 use_alpha(),
                                 texture_path);
    texture_name_ = texture_path;
    assert(glGetError() == GL_NO_ERROR);
};

void ZSpriteNode::draw(SpriteRenderer& renderer) {
    Texture2D tex = ResourceManager::GetTexture(texture_name_);
    assert(tex.ID != 0);
    renderer.DrawSprite(tex,
                        pos_,
                        size_,
                        0.0f,
                        glm::vec3(color_.x, color_.y, color_.z));
}

v8pp::class_<ZSpriteNode> ZSpriteNode::create(v8::Isolate* isolate) {
    v8pp::class_<ZSpriteNode> zsprite_node_class(isolate);
    zsprite_node_class
        .ctor<>()
        .inherit<ZNode>()
        .set("color", v8pp::property(
                        &ZSpriteNode::get_color,
                        &ZSpriteNode::set_color))
        .set("texture", v8pp::property(
                            &ZSpriteNode::get_texture,
                            &ZSpriteNode::set_texture))
        ;
    return zsprite_node_class;
}


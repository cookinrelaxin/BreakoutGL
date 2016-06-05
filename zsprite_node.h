#ifndef ZSPRITE_NODE_H
#define ZSPRITE_NODE_H

#include <string>

#include <glm/glm.hpp>

#include "znode.h"


class ZSpriteNode : public ZNode {
    public:
        ZSpriteNode();
        ~ZSpriteNode();
        //COLOR
        glm::vec4& get_color();
        void       set_color(glm::vec4 new_color);

        std::string get_texture();
        void        set_texture(std::string texture_path);

        void draw(SpriteRenderer& renderer);

        //Scripting
        static v8pp::class_<ZSpriteNode> create(v8::Isolate* isolate);

    protected:
        glm::vec4 color_;
        std::string texture_name_;
};

#endif

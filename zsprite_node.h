#ifndef ZSPRITE_NODE_H
#define ZSPRITE_NODE_H

#include <glm/glm.hpp>

class ZSpriteNode : public ZNode {
    public:
        ZSpriteNode();
        ~ZSpriteNode();
        //COLOR
        glm::vec4& color() { return color_; }

    private:
        glm::vec4 color_;
};

#endif

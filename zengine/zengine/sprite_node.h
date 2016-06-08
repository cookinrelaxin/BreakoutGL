#ifndef Z_SPRITE_NODE_H
#define Z_SPRITE_NODE_H

#include "node.h"

#include <string>

namespace Z {

class SpriteNode {
    public:
        SpriteNode();
        ~SpriteNode();

        glm::vec4 color();
        void color(glm::vec4 newColor);

        std::string texture();
        void texture(std::string texturePath);
    private:
    protected:
        void draw(SpriteRenderer& renderer) const;

        glm::vec4 color_;
        std::string texturePath_;
};

};

#endif

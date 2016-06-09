#ifndef Z_SPRITE_NODE_H
#define Z_SPRITE_NODE_H

#include "node.h"

#include <string>

#include "color_4.h"
#include "size_2.h"
#include "texture.h"

namespace Z {

class SpriteNode : public Node {
    public:
        SpriteNode();
        SpriteNode(size2       size,
                   pos2        position,
                   std::string texturePath,
                   color4      color,
                   float       rotation,
                   velocity2   velocity);
        ~SpriteNode();

        void setTexture(std::string texturePath);

        color4 color;
        std::string texturePath;
        float rotation;
    private:

    protected:
        void draw(SpriteRenderer* renderer);
        Texture2D texture;
};

};

#endif

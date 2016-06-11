#ifndef Z_TEXT_NODE_H
#define Z_TEXT_NODE_H

#include "sprite_node.h"

#include <string>

#include "color_4.h"
#include "size_2.h"
#include "texture.h"

namespace Z {

class TextNode : public Node {
    public:
        TextNode();
        ~TextNode();

        std::string text;
        color4 color;
        std::string font;
        unsigned int fontSize;
        bool centered;

        void hide();
        void show();

    private:

    protected:
        void draw(SpriteRenderer* spriteRenderer, TextRenderer* textRenderer);
};

};

#endif

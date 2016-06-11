#include "text_node.h"
#include "text_renderer.h"

namespace Z {
TextNode::TextNode()
    : text("DEFAULT TEXT")
    , font("./assets/fonts/OCRAEXT.TTF")
    , fontSize(10.0)
    , centered(false)
    , color(1.0, 1.0, 1.0, 1.0) {}

TextNode::~TextNode() {}
void TextNode::draw(SpriteRenderer* spriteRenderer, TextRenderer* textRenderer) {
    textRenderer->RenderText(text,
                             position.x + parent->position.x,
                             position.y + parent->position.y,
                             std::make_pair(font, fontSize),
                             centered,
                             glm::vec4(color.r, color.g, color.b, color.a));
                     
}

void TextNode::hide() {
    color.a = 0.0;
}

void TextNode::show() {
    color.a = 1.0;
}

};

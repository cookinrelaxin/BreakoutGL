#include "text_node.h"
#include "text_renderer.h"

namespace Z {
TextNode::TextNode() : text("DEFAULT TEXT"), fontSize(1.0), color(1.0, 1.0, 1.0, 1.0) {}
TextNode::~TextNode() {}
void TextNode::draw(SpriteRenderer* spriteRenderer, TextRenderer* textRenderer) {
    textRenderer->RenderText(text,
                             position.x + parent->position.x,
                             position.y + parent->position.y,
                             fontSize,
                             glm::vec4(color.r, color.g, color.b, color.a)
                             );
                     
}

};


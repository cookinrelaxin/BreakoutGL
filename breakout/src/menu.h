#ifndef MENU_H
#define MENU_H

#include "zengine/text_node.h"
#include "zengine/node.h"
#include "zengine/scene_node.h"

#include "zengine/pos_2.h"
#include "zengine/key_down_event.h"

class Menu : public Z::Node {
    public:
        Menu(std::string title,
             Z::size2 size);
        Z::SpriteNode* backgroundNode;
        Z::TextNode* caret;
        float minCaretPositionY;
        float maxCaretPositionY;
        Z::TextNode* titleNode;

        Z::pos2 nextOptionPosition;

        void addOption(Z::TextNode* option);
        void keyDown(Z::KeyDownEvent event);

    private:
};

#endif

#ifndef MENU_H
#define MENU_H

#include "zengine/text_node.h"
#include "zengine/node.h"
#include "zengine/scene_node.h"

class Menu : public Z::Node {
    public:
        Menu(std::string title,
             Z::size2 size);
        Z::SpriteNode* backgroundNode;
        Z::TextNode* titleNode;

        void show();
        void hide();

    private:
};

#endif



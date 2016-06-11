#include "menu.h"

Menu::Menu(std::string title,
           Z::size2 size) {
    this->size = size;

    titleNode = new Z::TextNode;
    titleNode->text = title;
    // titleNode->font = "
    titleNode->fontSize = 50.0;
    titleNode->centered = true;
    titleNode->position.x = size.width / 2;
    titleNode->position.y = 10;
    titleNode->zPosition = 2;
    addChild(titleNode);

    backgroundNode = new Z::SpriteNode;
    backgroundNode->size = size;
    backgroundNode->color = Z::color4(70.0 / 360.0, 137.0 / 360.0, 102.0 / 360.0, 1.0);
    addChild(backgroundNode);

    caret = new Z::TextNode;
    caret->text = ">";
    caret->color = Z::color4(0,0,0,0);
    caret->fontSize = 50.0;
    caret->position = Z::pos2(30, 65);
    caret->zPosition = 2;
    addChild(caret);

    minCaretPositionY = 70;
    nextOptionPosition = Z::pos2(70, 70);
}

void Menu::addOption(Z::TextNode* option) {
    addChild(option);
    option->position = nextOptionPosition;
    nextOptionPosition.y += 70;
    maxCaretPositionY = nextOptionPosition.y - 140;
}

void Menu::keyDown(Z::KeyDownEvent event) {
    switch (event.key()) {
        case Z::KeyInput::J: {
            if (caret->position.y <= maxCaretPositionY) 
                caret->position.y += 70;
            break;
        }
        case Z::KeyInput::K: {
            if (caret->position.y >= minCaretPositionY) 
                caret->position.y -= 70;
            break;
        }

        // case Z::KeyInput::M: {
        //     if (state == GameState::ACTIVE) {
        //         state = GameState::MENU;
        //         menu->show();
        //         TIME_SCALE = 0.0;
        //     }
        //     else {
        //         state = GameState::ACTIVE;
        //         menu->hide();
        //         TIME_SCALE = 1.0;
        //     }
        //     break;
        // }
    }
}

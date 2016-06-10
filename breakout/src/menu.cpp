#include "menu.h"

Menu::Menu(std::string title,
           Z::size2 size) {
    this->size = size;

    titleNode = new Z::TextNode;
    titleNode->text = title;
    titleNode->fontSize = 10.0;
    this->addChild(titleNode);

    backgroundNode = new Z::SpriteNode;
    backgroundNode->size = size;
    backgroundNode->color = Z::color4(0.1, 0.1, 0.1, 0.4);
    this->addChild(backgroundNode);
}

void Menu::show() {
    backgroundNode->color.a = 0.4;
    titleNode->color.a = 0.4;
}

void Menu::hide() {
    backgroundNode->color.a = 0.0;
    titleNode->color.a = 0.0;
}

// void Menu::addOption(Z::TextNode* option) {
//     options.push_back(option);
// }
//
// void Menu::addToScene(Z::SceneNode* scene) {
//     scene.addChild(title);
// }

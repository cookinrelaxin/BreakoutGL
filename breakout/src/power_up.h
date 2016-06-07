#ifndef POWER_UP_H
#define POWER_UP_H
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "game_object.h"

const glm::vec2 SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject {
    public:
        std::string Type;
        GLfloat Duration;
        GLboolean Activated;

        PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, Texture2D texture)
            : GameObject(position, SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated(GL_FALSE) {}
};

#endif

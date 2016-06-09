#include <iostream>
#include <fstream>
#include <sstream>

#include "zengine/zengine.h"
#include "zengine/velocity_2.h"

#include "json.hpp"
// #include <glm/glm.hpp>

#include "breakout.h"

using json = nlohmann::json;

const unsigned int SCREEN_WIDTH(1600), SCREEN_HEIGHT(1200);

struct Level {
    std::string name;
    std::vector<std::vector<unsigned short>> blockList;
    std::map<unsigned short, Z::color4> blockColors;
    std::map<unsigned short, std::string> blockTextures;
    Z::color4 ballColor;
    std::string ballTexture;
    Z::velocity2 initialBallVelocity;
    unsigned int ballRadius;
};

Z::SceneNode* Breakout::scene;
std::vector<std::shared_ptr<Level>> Breakout::levels;
std::vector<Z::SpriteNode*> Breakout::blocks;
std::shared_ptr<Z::SpriteNode> Breakout::paddle;
std::shared_ptr<Z::SpriteNode> Breakout::ball;
std::shared_ptr<Z::SpriteNode> Breakout::background;
std::shared_ptr<Level> Breakout::currentLevel;

bool MOVE_LEFT = false;
bool MOVE_RIGHT = false;
float PADDLE_SPEED = 20.0;
bool EXIT = false;
bool STUCK = true;

void Breakout::parseLevels(std::string levelFilePath) {
    std::ifstream file(levelFilePath);
    std::string str;
    std::string file_contents;
    while (std::getline(file, str)) {
        file_contents += str;
        file_contents.push_back('\n');
    }

    json json_levels = json::parse(file_contents.c_str());

    for (auto& jl : json_levels["levels"]) {
        if (!(jl.is_object()))
            throw std::runtime_error("Level parse error: expected level to be an object");
        std::shared_ptr<Level> level = std::make_shared<Level>();
        level->name = jl["name"];
        for (auto& row : jl["blocks"]["list"]) {
            std::vector<unsigned short> blockRow;
            for (auto& col : row) {
                blockRow.push_back(col);
            }
            level->blockList.push_back(blockRow);
        }

        {
            auto colors(jl["blocks"]["rgb_colors"]);
            for (json::iterator it = colors.begin(); it != colors.end(); ++it) {
                unsigned int  key = std::stoi(it.key());
                auto val = it.value();

                Z::color4 color;
                color.r = val["red"];
                color.r /= 360.0;

                color.g = val["green"];
                color.g /= 360.0;

                color.b = val["blue"];
                color.b /= 360.0;

                color.a = 1.0;

                level->blockColors[key] = color;
            }
        }

        {
            auto textures(jl["blocks"]["textures"]);
            for (json::iterator it = textures.begin(); it != textures.end(); ++it) {
                unsigned int key = std::stoi(it.key());
                std::string val = it.value();

                level->blockTextures[key] = val;
            }
        }

        auto ball(jl["ball"]);

        {
            Z::color4 color;
            color.r = ball["color"]["red"];
            color.r /= 360.0;

            color.g = ball["color"]["green"];
            color.g /= 360.0;

            color.b = ball["color"]["blue"];
            color.b /= 360.0;

            color.a = 1.0;
            level->ballColor = color;
        }

        level->ballTexture = ball["texture"];;

        level->initialBallVelocity.dx = ball["initial_velocity"]["x"];
        level->initialBallVelocity.dy = ball["initial_velocity"]["y"];

        level->ballRadius = ball["radius"];
        levels.push_back(level);
    }
}

void Breakout::loadLevel(int levelNumber, Z::SceneNode* scene) {
    std::shared_ptr<Level> level = levels.at(levelNumber);
    currentLevel = level;
    int x(0), y(0);
    float blockWidth = scene->size.width / level->blockList[0].size();
    float blockHeight = .7 * blockWidth;
    for (auto row : level->blockList) {
        x = 0;
        for (auto col : row) {
             Z::SpriteNode* block = new Z::SpriteNode;
             block->setTexture(level->blockTextures[col]);
             block->zPosition = 1;
             block->color = level->blockColors[col];
             block->size = Z::size2(blockWidth, blockHeight);
             block->position = Z::pos2(x,y);
             blocks.push_back(block);
             scene->addChild(block);

             x += blockWidth;
        }
        y += blockHeight;
    }

    ball = std::make_shared<Z::SpriteNode>();
    ball->zPosition = 2;
    ball->color = level->ballColor;
    ball->size = Z::size2(level->ballRadius, level->ballRadius);
    ball->position = Z::pos2(paddle->position.x + paddle->size.width / 2 - ball->size.width / 2,
                             paddle->position.y - ball->size.height);
    ball->setTexture(level->ballTexture);
    scene->addChild(ball.get());

}

Z::SceneNode* Breakout::init() {
    std::cout << "init" << std::endl;

    scene = new Z::SceneNode();
    scene->size.width = 1024;
    scene->size.height = 768;
    scene->backgroundColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

    parseLevels("./assets/levels.json");

    paddle = std::make_shared<Z::SpriteNode>();
    paddle->size = Z::size2(150, 30);
    paddle->position = Z::pos2(scene->size.width / 2 - paddle->size.width / 2,
                              scene->size.height - paddle->size.height);
                           
    paddle->rotation = 0;
    paddle->zPosition = 2;
    paddle->velocity = Z::velocity2(0, 0);;
    paddle->color = Z::color4(1.0, 1.0, 1.0, 1.0);
    paddle->setTexture("./assets/textures/paddle.png");

    scene->addChild(paddle.get());

    background = std::make_shared<Z::SpriteNode>();
    background->color = Z::color4(1.0, 0.0, 0.0, 0.0);
    background->size = scene->size;
    background->zPosition = -1;
    background->setTexture("./assets/textures/background.png");

    scene->addChild(background.get());

    loadLevel(0, scene);

    return scene;
}

bool Breakout::update(float dt) {
    if (EXIT) return false;
    if (MOVE_LEFT) {
        paddle->position.x -= PADDLE_SPEED;
        if (paddle->position.x <= 0) {
            paddle->position.x = 0;
        }
        if (STUCK)
            ball->position = Z::pos2(
                    paddle->position.x + paddle->size.width / 2 - ball->size.width / 2,
                    paddle->position.y - ball->size.height);
    }

    if (MOVE_RIGHT) {
        paddle->position.x += PADDLE_SPEED;
        if (paddle->position.x + paddle->size.width >= scene->size.width) {
            paddle->position.x = scene->size.width - paddle->size.width;
        }
        if (STUCK)
            ball->position = Z::pos2(
                    paddle->position.x + paddle->size.width / 2 - ball->size.width / 2,
                    paddle->position.y - ball->size.height);
    }

    if (!STUCK) {
        ball->position.x += ball->velocity.dx;
        ball->position.y += ball->velocity.dy;
        ball->velocity.dy += .1f;
    }

    if (ball->position.x + ball->size.width >= scene->size.width) {
        ball->velocity.dx = -ball->velocity.dx;
    }

    if (ball->position.x <= 0) {
        ball->velocity.dx = -ball->velocity.dx;
    }

    if (ball->position.y <= 0) {
        ball->velocity.dy = -ball->velocity.dy;
    }
    return true;
}

void Breakout::shutdown() {
    std::cout << "shutdown" << std::endl;
}

void Breakout::mouseDown(Z::MouseDownEvent event) {
    std::cout << "mouseDown" << std::endl;
}

void Breakout::mouseUp(Z::MouseUpEvent event) {
    std::cout << "mouseUp" << std::endl;
}

void Breakout::mouseMove(Z::MouseMoveEvent event) {
    std::cout << "mouseMove dx: " << event.dx() << std::endl;
    std::cout << "mouseMove dy: " << event.dy() << std::endl;
}

void Breakout::keyDown(Z::KeyDownEvent event) {
    switch (event.key()) {
        case Z::KeyInput::A: {
            MOVE_LEFT = true;
            break;
        }
        case Z::KeyInput::D: {
            MOVE_RIGHT = true;
            break;
        }
        case Z::KeyInput::ESCAPE: {
            EXIT = true;
            break;
        }
        case Z::KeyInput::SPACE: {
            if (STUCK) {
                STUCK = false;
                ball->velocity = currentLevel->initialBallVelocity;
            }
            break;
        }
    }
}

void Breakout::keyUp(Z::KeyUpEvent event) {
    switch (event.key()) {
        case Z::KeyInput::A: {
            MOVE_LEFT = false;
            break;
        }
        case Z::KeyInput::D: {
            MOVE_RIGHT = false;
            break;
        }
    }
}

void Breakout::doCollisions() {
    // for (SpriteNode *brick : blocks) {
    //     if (!box.Destroyed) {
    //         Collision collision = CheckCollision(*Ball, box);
    //         if (std::get<0>(collision)) {
    //             if (!box.IsSolid) {
    //                 box.Destroyed = GL_TRUE;
    //                 this->SpawnPowerUps(box);
    //                 SoundEngine->play2D("sounds/bleep.wav", GL_FALSE);
    //             }
    //             else {
    //                 ShakeTime = 0.05f;
    //                 Effects->Shake = GL_TRUE;
    //                 SoundEngine->play2D("sounds/solid.wav", GL_FALSE);
    //             }
    //             Direction dir = std::get<1>(collision);
    //             glm::vec2 diff_vector = std::get<2>(collision);
    //             if (!(Ball->PassThrough && !box.IsSolid)) {
    //                 if (dir == LEFT || dir == RIGHT) {
    //                     Ball->Velocity.x = -Ball->Velocity.x;
    //                     GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
    //                     if (dir == LEFT)
    //                         Ball->Position.x += penetration;
    //                     else
    //                         Ball->Position.x -= penetration;
    //                 }
    //                 else {
    //                     Ball->Velocity.y = -Ball->Velocity.y;
    //                     GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
    //                     if (dir == UP)
    //                         Ball->Position.y -= penetration;
    //                     else
    //                         Ball->Position.y += penetration;
    //                 }
    //             }
    //         }
    //     }
    // }
    // for (PowerUp &powerUp : this->PowerUps)
    //     if (!powerUp.Destroyed) {
    //         if (powerUp.Position.y >= this->Height)
    //             powerUp.Destroyed = GL_TRUE;
    //         if (CheckCollisionAABBAABB(*Player, powerUp)) {
    //             ActivatePowerUp(powerUp);
    //             powerUp.Destroyed = GL_TRUE;
    //             powerUp.Activated = GL_TRUE;
    //             SoundEngine->play2D("sounds/powerup.wav", GL_FALSE);
    //         }
    //     }

    // Collision result = CheckCollision(*Ball, *Player);
    // if (!Ball->Stuck && std::get<0>(result)) {
    //     GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
    //     GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
    //     GLfloat percentage = distance / (Player->Size.x / 2);
    //     GLfloat strength(2.0f);
    //     glm::vec2 oldVelocity = Ball->Velocity;
    //     Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
    //     Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
    //     Ball->Velocity.y = -1 * abs(Ball->Velocity.y);

    //     Ball->Stuck = Ball->Sticky;
    //     SoundEngine->play2D("sounds/bleep.wav", GL_FALSE);
    // }
}

Collision Breakout::CheckCollision(std::shared_ptr<Z::SpriteNode> ball,
                                   std::shared_ptr<Z::SpriteNode> other) {
    // Z::vec2 center(ball.position + one.Radius);

    // glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    // glm::vec2 aabb_center(
    //         two.Position.x + aabb_half_extents.x,
    //         two.Position.y + aabb_half_extents.y
    // );
    // glm::vec2 difference = center - aabb_center;
    // glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // glm::vec2 closest = aabb_center + clamped;
    // glm::vec2 newDifference = closest - center;

    // if (glm::length(newDifference) < one.Radius)
    //     return std::make_tuple(GL_TRUE, VectorDirection(newDifference), newDifference);
    // else
    //     return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}



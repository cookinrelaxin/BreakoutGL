#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "game_level.h"
#include "ball_object.h"
#include "power_up.h"

#include <vector>
#include <tuple>

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

const glm::vec2 PLAYER_SIZE(200, 30);
const GLfloat PLAYER_VELOCITY(1500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(400.f, -1000.0f);
const GLfloat BALL_RADIUS = 40.0f;

class Game {
    public:
        GameState State;
        GLboolean Keys[1024];
        GLboolean KeysProcessed[1024];
        GLuint Width, Height;
        std::vector<GameLevel> Levels;
        GLuint Level;
        GLuint Lives;
        std::vector<PowerUp> PowerUps;

        Game(GLuint width, GLuint height);
        ~Game();

        void Init();
        void ProcessInput(GLfloat dt);
        void Update(GLfloat dt);
        void Render();
    private:
        void DoCollisions();
        GLboolean CheckCollisionAABBAABB(GameObject &one, GameObject &two);
        GLboolean CheckCollisionAABBCircle(BallObject &one, GameObject &two);
        Collision CheckCollision(BallObject &one, GameObject &two);
        Direction VectorDirection(glm::vec2 target);
        void ResetLevel();
        void ResetPlayer();

        void SpawnPowerUps(GameObject &block);
        void UpdatePowerUps(GLfloat dt);
        void ActivatePowerUp(PowerUp &powerUp);
        GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);
};

#endif

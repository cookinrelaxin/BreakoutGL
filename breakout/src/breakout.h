#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "zengine/zengine.h"
#include "zengine/sprite_node.h"
#include "zengine/vec_2.h"

#include <vector>
#include <tuple>

struct Level;

enum GameState {
    ACTIVE,
    MENU,
    WIN,
    LOSE
};

enum class Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, Z::vec2> Collision;

class Breakout {
    public:
        static Z::SceneNode* init();
        static bool update(float dt);
        static void shutdown();

        static void mouseDown(Z::MouseDownEvent event);
        static void mouseUp(Z::MouseUpEvent event);
        static void mouseMove(Z::MouseMoveEvent event);

        static void keyDown(Z::KeyDownEvent event);
        static void keyUp(Z::KeyUpEvent event);
    private:
        static Z::SceneNode* scene;
        static std::shared_ptr<Z::SpriteNode> background;

        static std::vector<std::shared_ptr<Level>> levels;
        static std::vector<Z::SpriteNode*> blocks;
        static std::shared_ptr<Z::SpriteNode> paddle;
        static std::shared_ptr<Z::SpriteNode> ball;
        static std::shared_ptr<Level> currentLevel;

        static void parseLevels(std::string levelFilePath);
        static void loadLevel(int levelNumber, Z::SceneNode* scene);
        static void doCollisions();
        static Collision CheckCollision(std::shared_ptr<Z::SpriteNode> ball,
                                        std::shared_ptr<Z::SpriteNode> other);
        static Direction VectorDirection(Z::vec2 target);

};

#endif

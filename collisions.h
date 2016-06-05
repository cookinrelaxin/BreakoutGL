#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "znode.h"

#include <glm/glm.hpp>

#include <tuple>

#include <v8pp/class.hpp>

enum class Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

class Collisions {
    public:
        Collisions() = delete;
        ~Collisions() = delete;
        // void DoCollisions();
        static bool CheckCollisionAABBAABB(ZNode* one, ZNode* two);
        static GLboolean CheckCollisionAABBCircle(ZNode* aabb, ZNode* circle);
        static Collision CheckCollision(ZNode* one, ZNode* two);
        static Direction VectorDirection(glm::vec2 target);
};

class ZCollision {
    public:
        ZCollision(bool exists, std::string direction, glm::vec2 difference);
        ~ZCollision();
        static v8pp::class_<ZCollision> create(v8::Isolate* isolate);

        bool get_exists();
        void set_exists(bool newExists);
        std::string get_direction();
        void set_direction(std::string newDirection);
        glm::vec2& get_difference();
        void set_difference(glm::vec2 newDifference);
    private:
        bool exists_;
        std::string direction_;
        glm::vec2 difference_;
};

#endif

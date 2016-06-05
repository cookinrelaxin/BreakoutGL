#include "collisions.h"

#include "shell.h"

#include <iostream>

// void Game::DoCollisions() {
//     for (GameObject &box : this->Levels[this->Level].Bricks) {
//         if (!box.Destroyed) {
//             Collision collision = CheckCollision(*Ball, box);
//             if (std::get<0>(collision)) {
//                 if (!box.IsSolid) {
//                     box.Destroyed = GL_TRUE;
//                     this->SpawnPowerUps(box);
//                     SoundEngine->play2D("sounds/bleep.wav", GL_FALSE);
//                 }
//                 else {
//                     ShakeTime = 0.05f;
//                     Effects->Shake = GL_TRUE;
//                     SoundEngine->play2D("sounds/solid.wav", GL_FALSE);
//                 }
//                 Direction dir = std::get<1>(collision);
//                 glm::vec2 diff_vector = std::get<2>(collision);
//                 if (!(Ball->PassThrough && !box.IsSolid)) {
//                     if (dir == LEFT || dir == RIGHT) {
//                         Ball->Velocity.x = -Ball->Velocity.x;
//                         GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
//                         if (dir == LEFT)
//                             Ball->Position.x += penetration;
//                         else
//                             Ball->Position.x -= penetration;
//                     }
//                     else {
//                         Ball->Velocity.y = -Ball->Velocity.y;
//                         GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
//                         if (dir == UP)
//                             Ball->Position.y -= penetration;
//                         else
//                             Ball->Position.y += penetration;
//                     }
//                 }
//             }
//         }
//     }
//     for (PowerUp &powerUp : this->PowerUps)
//         if (!powerUp.Destroyed) {
//             if (powerUp.Position.y >= this->Height)
//                 powerUp.Destroyed = GL_TRUE;
//             if (CheckCollisionAABBAABB(*Player, powerUp)) {
//                 ActivatePowerUp(powerUp);
//                 powerUp.Destroyed = GL_TRUE;
//                 powerUp.Activated = GL_TRUE;
//                 SoundEngine->play2D("sounds/powerup.wav", GL_FALSE);
//             }
//         }
// 
//     Collision result = CheckCollision(*Ball, *Player);
//     if (!Ball->Stuck && std::get<0>(result)) {
//         GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
//         GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
//         GLfloat percentage = distance / (Player->Size.x / 2);
//         GLfloat strength(2.0f);
//         glm::vec2 oldVelocity = Ball->Velocity;
//         Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
//         Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
//         Ball->Velocity.y = -1 * abs(Ball->Velocity.y);
// 
//         Ball->Stuck = Ball->Sticky;
//         SoundEngine->play2D("sounds/bleep.wav", GL_FALSE);
//     }
// }

bool Collisions::CheckCollisionAABBAABB(ZNode* one, ZNode* two) {
    glm::vec2 pos1 = one->get_position();
    glm::vec2 size1 = one->get_size();
    glm::vec2 pos2 = two->get_position();
    glm::vec2 size2 = two->get_size();
    bool collisionX = pos1.x + size1.x >= pos2.x &&
                      pos2.x + size2.x >= pos1.x;

    bool collisionY = pos1.y + size1.y >= pos2.y &&
                      pos2.y + size2.y >= pos1.y;

    return collisionX && collisionY;
}

// GLboolean Game::CheckCollisionAABBCircle(BallObject &one, GameObject &two) {
//     glm::vec2 center(one.Position + one.Radius);
// 
//     glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
//     glm::vec2 aabb_center(
//             two.Position.x + aabb_half_extents.x,
//             two.Position.y + aabb_half_extents.y
//     );
//     glm::vec2 difference = center - aabb_center;
//     glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
//     glm::vec2 closest = aabb_center + clamped;
//     return glm::length(closest - center) < one.Radius;
// }
// 
Collision Collisions::CheckCollision(ZNode* one, ZNode* two) {
    glm::vec2 pos1 = one->get_position();
    float radius1 = one->get_size().x / 2;

    glm::vec2 pos2 = two->get_position();
    glm::vec2 size2 = two->get_size();

    glm::vec2 center(pos1 + radius1);

    glm::vec2 aabb_half_extents(size2.x / 2, size2.y / 2);
    glm::vec2 aabb_center(
            pos2.x + aabb_half_extents.x,
            pos2.y + aabb_half_extents.y
    );
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;
    glm::vec2 newDifference = closest - center;

    if (glm::length(newDifference) < radius1)
        return std::make_tuple(GL_TRUE, VectorDirection(newDifference), newDifference);
    else
        return std::make_tuple(GL_FALSE, Direction::UP, glm::vec2(0, 0));
}
// 
Direction Collisions::VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, -1.0f),
        glm::vec2(-1.0f, 0.0f),
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i(0); i < 4; i++) {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

ZCollision::ZCollision(bool exists, std::string direction, glm::vec2 difference)
    : exists_(exists)
    , direction_(direction)
    , difference_(difference) {

    // std::cout << "ONE" << std::endl;
    v8::Isolate* isolate = Shell::_context->isolate();
    // std::cout << "TWO" << std::endl;
    // v8pp::class_<bool>::reference_external(isolate, &exists_);
    // std::cout << "THREE" << std::endl;
    // v8pp::class_<std::string>::reference_external(isolate, &direction_);
    // std::cout << "FOUR" << std::endl;
    v8pp::class_<glm::vec2>::reference_external(isolate, &difference_);
    // std::cout << "FIVE" << std::endl;
}

ZCollision::~ZCollision() {
    v8::Isolate* isolate = Shell::_context->isolate();
    // v8pp::class_<bool>::unreference_external(isolate, &exists_);
    // v8pp::class_<std::string>::unreference_external(isolate, &direction_);
    v8pp::class_<glm::vec2>::unreference_external(isolate, &difference_);
}

bool ZCollision::get_exists() {
    return exists_; 
}

void ZCollision::set_exists(bool newExists) {
    exists_ = newExists;
}

std::string ZCollision::get_direction() {
    return direction_; 
}

void ZCollision::set_direction(std::string newDirection) {
    direction_ = newDirection;
}

glm::vec2& ZCollision::get_difference() {
    return difference_; 
}

void ZCollision::set_difference(glm::vec2 newDifference) {
    difference_ = newDifference;
}

v8pp::class_<ZCollision> ZCollision::create(v8::Isolate* isolate) {
    v8pp::class_<ZCollision> zcollision_class(isolate);
    zcollision_class
        .ctor<bool, std::string, glm::vec2>()
        .set("exists", v8pp::property(&ZCollision::get_exists,
                                      &ZCollision::set_exists))
        .set("direction", v8pp::property(&ZCollision::get_direction,
                                         &ZCollision::set_direction))
        .set("difference", v8pp::property(&ZCollision::get_difference,
                                          &ZCollision::set_difference))
        ;
    return zcollision_class;
}

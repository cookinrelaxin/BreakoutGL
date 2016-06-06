#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"

#include <iostream>
#include <algorithm>
#include <sstream>

#include <irrklang/irrKlang.h>
using namespace irrklang;

ISoundEngine *SoundEngine = createIrrKlangDevice();

SpriteRenderer    *Renderer;
GameObject        *Player;
BallObject        *Ball;
ParticleGenerator *Particles;
PostProcessor     *Effects;
GLfloat           ShakeTime = 0.0f;
TextRenderer      *Text;

Game::Game(GLuint width, GLuint height)
    : State(GAME_MENU)
    , Keys()
    , Width(width)
    , Height(height) 
    , Level(0) 
    , Lives(3) {}

Game::~Game() {
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Effects;
    delete Text;
    SoundEngine->drop();
}

void Game::Init() {
    // Load shaders
    ResourceManager::LoadShader("shaders/sprite.vs",
                                "shaders/sprite.fs",
                                nullptr,
                                "sprite");
    ResourceManager::LoadShader("shaders/particle.vs",
                                "shaders/particle.fs",
                                nullptr,
                                "particle");
    ResourceManager::LoadShader("shaders/post_process.vs",
                                "shaders/post_process.fs",
                                nullptr,
                                "postprocessing");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<GLfloat>(this->Width), 
                                      static_cast<GLfloat>(this->Height),
                                      0.0f,
                                      -1.0f,
                                      1.0f);

    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // Load textures
    ResourceManager::LoadTexture("textures/background.jpg",
                                 GL_FALSE,
                                 "background");
    ResourceManager::LoadTexture("textures/awesomeface.png",
                                 GL_TRUE,
                                 "face");
    ResourceManager::LoadTexture("textures/block.png",
                                 GL_FALSE,
                                 "block");
    ResourceManager::LoadTexture("textures/block_solid.png",
                                 GL_FALSE,
                                 "block_solid");
    ResourceManager::LoadTexture("textures/paddle.png",
                                 GL_TRUE,
                                 "paddle");
    ResourceManager::LoadTexture("textures/particle.png",
                                 GL_TRUE,
                                 "particle");
    ResourceManager::LoadTexture("textures/powerup_speed.png",
                                 GL_TRUE,
                                 "powerup_speed");
    ResourceManager::LoadTexture("textures/powerup_sticky.png",
                                 GL_TRUE,
                                 "powerup_sticky");
    ResourceManager::LoadTexture("textures/powerup_increase.png",
                                 GL_TRUE,
                                 "powerup_increase");
    ResourceManager::LoadTexture("textures/powerup_confuse.png",
                                 GL_TRUE,
                                 "powerup_confuse");
    ResourceManager::LoadTexture("textures/powerup_chaos.png",
                                 GL_TRUE,
                                 "powerup_chaos");
    ResourceManager::LoadTexture("textures/powerup_passthrough.png",
                                 GL_TRUE,
                                 "powerup_passthrough");

    // Set render-specific controls
    Shader sh = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(sh);
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"),
                                      ResourceManager::GetTexture("particle"),
                                      500);
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"),
                                this->Width,
                                this->Height
    );
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("fonts/OCRAEXT.TTF", 24);

    // Load levels
    GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height * 0.5);
    GameLevel two; two.Load("levels/two.lvl", this->Width, this->Height * 0.5);
    GameLevel three; three.Load("levels/three.lvl", this->Width, this->Height * 0.5);
    GameLevel four; four.Load("levels/four.lvl", this->Width, this->Height * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

    // Configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2,
                                    this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos,
                            PLAYER_SIZE,
                            ResourceManager::GetTexture("paddle"));

    glm::vec2 ballPos =
        playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Ball = new BallObject(ballPos,
                          BALL_RADIUS,
                          INITIAL_BALL_VELOCITY,
                          ResourceManager::GetTexture("face")
    );

    SoundEngine->loadPlugins("ikpMP3.dylib");
    SoundEngine->play2D("./sounds/breakout.mp3", GL_TRUE);
}

void Game::Update(GLfloat dt) {
    Ball->Move(dt, this->Width);
    this->DoCollisions();
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));
    this->UpdatePowerUps(dt);
    if (ShakeTime > 0.0f) {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = GL_FALSE;
    }
    if (Ball->Position.y >= this->Height) {
        --this->Lives;
        if (this->Lives == 0) {
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }
    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()) {
        this->ResetLevel();
        this->ResetPlayer();
        Effects->Chaos = GL_TRUE;
        this->State = GAME_WIN;
    }
}

void Game::DoCollisions() {
    for (GameObject &box : this->Levels[this->Level].Bricks) {
        if (!box.Destroyed) {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) {
                if (!box.IsSolid) {
                    box.Destroyed = GL_TRUE;
                    this->SpawnPowerUps(box);
                    SoundEngine->play2D("sounds/bleep.wav", GL_FALSE);
                }
                else {
                    ShakeTime = 0.05f;
                    Effects->Shake = GL_TRUE;
                    SoundEngine->play2D("sounds/solid.wav", GL_FALSE);
                }
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (!(Ball->PassThrough && !box.IsSolid)) {
                    if (dir == LEFT || dir == RIGHT) {
                        Ball->Velocity.x = -Ball->Velocity.x;
                        GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
                        if (dir == LEFT)
                            Ball->Position.x += penetration;
                        else
                            Ball->Position.x -= penetration;
                    }
                    else {
                        Ball->Velocity.y = -Ball->Velocity.y;
                        GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
                        if (dir == UP)
                            Ball->Position.y -= penetration;
                        else
                            Ball->Position.y += penetration;
                    }
                }
            }
        }
    }
    for (PowerUp &powerUp : this->PowerUps)
        if (!powerUp.Destroyed) {
            if (powerUp.Position.y >= this->Height)
                powerUp.Destroyed = GL_TRUE;
            if (CheckCollisionAABBAABB(*Player, powerUp)) {
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = GL_TRUE;
                powerUp.Activated = GL_TRUE;
                SoundEngine->play2D("sounds/powerup.wav", GL_FALSE);
            }
        }

    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result)) {
        GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
        GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        GLfloat percentage = distance / (Player->Size.x / 2);
        GLfloat strength(2.0f);
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
        Ball->Velocity.y = -1 * abs(Ball->Velocity.y);

        Ball->Stuck = Ball->Sticky;
        SoundEngine->play2D("sounds/bleep.wav", GL_FALSE);
    }
}

GLboolean Game::CheckCollisionAABBAABB(GameObject &one, GameObject &two) {
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;

    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;

    return collisionX && collisionY;
}

GLboolean Game::CheckCollisionAABBCircle(BallObject &one, GameObject &two) {
    glm::vec2 center(one.Position + one.Radius);

    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center(
            two.Position.x + aabb_half_extents.x,
            two.Position.y + aabb_half_extents.y
    );
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;
    return glm::length(closest - center) < one.Radius;
}

Collision Game::CheckCollision(BallObject &one, GameObject &two) {
    glm::vec2 center(one.Position + one.Radius);

    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center(
            two.Position.x + aabb_half_extents.x,
            two.Position.y + aabb_half_extents.y
    );
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;
    glm::vec2 newDifference = closest - center;

    if (glm::length(newDifference) < one.Radius)
        return std::make_tuple(GL_TRUE, VectorDirection(newDifference), newDifference);
    else
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

Direction Game::VectorDirection(glm::vec2 target) {
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

void Game::ProcessInput(GLfloat dt) {
    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
            this->Level = (this->Level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
        }
        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]) {
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
            this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
        }
    }
    if (this->State == GAME_WIN) {
        if (this->Keys[GLFW_KEY_ENTER]) {
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
            Effects->Chaos = GL_FALSE;
            this->State = GAME_MENU;
        }
    }
    if (this->State == GAME_ACTIVE) {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        if (this->Keys[GLFW_KEY_A]) {
            if (Player->Position.x >= 0) {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D]) {
            if (Player->Position.x <= this->Width - Player->Size.x) {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE]) {
            Ball->Stuck = false;
        }
    }
}

void Game::Render() {
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN) {
        Effects->BeginRender();
            Texture2D bg = ResourceManager::GetTexture("background");
            Renderer->DrawSprite(bg,
                                 glm::vec2(0, 0),
                                 glm::vec2(this->Width, this->Height),
                                 0.0f);
            this->Levels[this->Level].Draw(*Renderer);
            Player->Draw(*Renderer);
            for (PowerUp &powerUp : this->PowerUps)
                if (!powerUp.Destroyed)
                    powerUp.Draw(*Renderer);
            Ball->Draw(*Renderer);
            Particles->Draw();
        Effects->EndRender();
        Effects->Render(glfwGetTime());
        std::stringstream ss; ss << this->Lives;
        Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
    }
    int xOffset(10*16);
    if (this->State == GAME_MENU) {
        Text->RenderText("Press ENTER to start",
                         this->Width / 2 - xOffset,
                         this->Height / 2,
                         1.0f,
                         glm::vec3(1.0f, 1.0f, 0.0f),
                         GL_TRUE
        );
        Text->RenderText("Press W or S to select level",
                         this->Width / 2 - xOffset,
                         this->Height / 2 + 20.0f,
                         0.75f,
                         glm::vec3(1.0f, 1.0f, 0.0f),
                         GL_TRUE
        );
    }
    if (this->State == GAME_WIN) {
        Text->RenderText("You WON!!!",
                         this->Width / 2 - xOffset,
                         this->Height / 2 - 20.0f,
                         1.0f,
                         glm::vec3(0.0f, 1.0f, 0.0f),
                         GL_TRUE
        );
        Text->RenderText("Press ENTER to restart or ESC to quit",
                         this->Width / 2 - xOffset,
                         this->Height / 2,
                         1.0f,
                         glm::vec3(1.0f, 1.0f, 0.0f),
                         GL_TRUE
        );
    }
}

void Game::ResetLevel() {
    if (this->Level == 0)
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height * 0.5f);
    else if (this->Level == 1)
        this->Levels[0].Load("levels/two.lvl", this->Width, this->Height * 0.5f);
    else if (this->Level == 2)
        this->Levels[0].Load("levels/three.lvl", this->Width, this->Height * 0.5f);
    else if (this->Level == 3)
        this->Levels[0].Load("levels/four.lvl", this->Width, this->Height * 0.5f);
    this->Lives = 3;
}

void Game::ResetPlayer() {
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2,
                                 this->Height - PLAYER_SIZE.y);
    Ball->Reset(
            Player->Position + glm::vec2(
                PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)),
            INITIAL_BALL_VELOCITY);
    Effects->Chaos = Effects->Confuse = GL_FALSE;
    Ball->PassThrough = Ball->Sticky = GL_FALSE;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}

void Game::UpdatePowerUps(GLfloat dt) {
    for (PowerUp &powerUp : this->PowerUps) {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated) {
            powerUp.Duration -= dt;
            if (powerUp.Duration <= 0.0f) {
                powerUp.Activated = GL_FALSE;
                if (powerUp.Type == "sticky") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky")) {
                        Ball->Sticky = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                if (powerUp.Type == "pass-through") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through")) {
                        Ball->PassThrough = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                if (powerUp.Type == "confuse") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse")) {
                        Effects->Confuse = GL_FALSE;
                    }
                }
                if (powerUp.Type == "chaos") {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos")) {
                        Effects->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    this->PowerUps.erase(
            std::remove_if(this->PowerUps.begin(),
                           this->PowerUps.end(),
                           [](const PowerUp &powerUp) {
                               return powerUp.Destroyed && !powerUp.Activated;
                           }
            ),
            this->PowerUps.end()
    );
}

GLboolean ShouldSpawn(GLuint chance) {
    GLuint random = rand() % chance;
    return random == 0;
}

void Game::SpawnPowerUps(GameObject &block) {
    const GLuint chance(100);
    if (ShouldSpawn(chance))
        this->PowerUps.push_back(
                PowerUp("speed",
                        glm::vec3(0.5f, 0.5f, 1.0f),
                        0.0f,
                        block.Position,
                        ResourceManager::GetTexture("powerup_speed")
                )
        );
    if (ShouldSpawn(chance))
        this->PowerUps.push_back(
                PowerUp("sticky",
                        glm::vec3(1.0f, 0.5f, 1.0f),
                        20.0f,
                        block.Position,
                        ResourceManager::GetTexture("powerup_sticky")
                )
        );
    if (ShouldSpawn(chance))
        this->PowerUps.push_back(
                PowerUp("pass-through",
                        glm::vec3(0.5f, 1.0f, 0.5f),
                        10.0f,
                        block.Position,
                        ResourceManager::GetTexture("powerup_passthrough")
                )
        );
    if (ShouldSpawn(chance))
        this->PowerUps.push_back(
                PowerUp("pad-size-increase",
                        glm::vec3(1.0f, 0.6f, 0.4f),
                        0.0f,
                        block.Position,
                        ResourceManager::GetTexture("powerup_increase")
                )
        );
    if (ShouldSpawn(chance))
        this->PowerUps.push_back(
                PowerUp("confuse",
                        glm::vec3(1.0f, 0.3f, 0.3f),
                        15.0f,
                        block.Position,
                        ResourceManager::GetTexture("powerup_confuse")
                )
        );
    if (ShouldSpawn(chance))
        this->PowerUps.push_back(
                PowerUp("chaos",
                        glm::vec3(0.9f, 0.25f, 0.25f),
                        15.0f,
                        block.Position,
                        ResourceManager::GetTexture("powerup_chaos")
                )
        );
}

void Game::ActivatePowerUp(PowerUp &powerUp) {
    if (powerUp.Type == "speed") {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky") {
        Ball->Sticky = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through") {
        Ball->PassThrough = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase") {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse") {
        if (!Effects->Chaos)
            Effects->Confuse = GL_TRUE;
    }
    else if (powerUp.Type == "chaos") {
        if (!Effects->Confuse)
            Effects->Chaos = GL_TRUE;
    }
}

GLboolean Game::IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type) {
    for (const PowerUp &powerUp : powerUps) {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

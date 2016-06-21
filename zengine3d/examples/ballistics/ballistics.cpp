#include <iostream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "particle.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

GLuint screenWidth(800), screenHeight(600);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void error_callback(int errorCode, const char* description);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void Do_Movement();
GLuint loadTexture(GLchar* path);

enum class ShotType {
    PISTOL,
    ARTILLERY,
    FIREBALL,
    LASER
};

struct AmmoRound {
    AmmoRound(ShotType type) : type(type) {
        GLfloat vertices[] = {
            // Positions           
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
        };	
    }
    void fire() {
        switch (type) {
            case PISTOL: 
                particle.mass = 2.0f;
                particle.velocity = glm::vec3(0.0f, 0.0f, 35.0f);
                particle.acceleration = glm::vec3(0.0f, -1.0f, 0.0f);
                particle.damping = 0.99f;
                break;
            case ARTILLERY: 
                particle.mass = 200.0f;
                particle.velocity = glm::vec3(0.0f, 30.0f, 40.0f);
                particle.acceleration = glm::vec3(0.0f, -20.0f, 0.0f);
                particle.damping = 0.99f;
                break;
            case FIREBALL: 
                particle.mass = 1.0f;
                particle.velocity = glm::vec3(0.0f, 0.0f, 10.0f);
                particle.acceleration = glm::vec3(0.0f, 0.6f, 0.0f);
                particle.damping = 0.9f;
                break;
            case LASER: 
                particle.mass = 0.1f;
                particle.velocity = glm::vec3(0.0f, 0.0f, 100.0f);
                particle.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
                particle.damping = 0.99f;
                break;
        };
    }
    void render(Shader& shader) {
    }
    Particle particle;
    ShotType type;
    float startTime;
};

void fire();
void update();
ShotType currentShotType = ShotType::PISTOL;
const unsigned int ammoRounds = 16;
AmmoRound ammo[ammoRounds];

Camera camera(CameraStyle::NOCLIP, glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
bool keysPressed[1024];
GLfloat lastX(400), lastY(300);
bool firstMouse = true;

GLfloat deltaTime(0.0f);
GLfloat lastFrame(0.0f);

int main() {

    {
        glfwInit();
        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
    }

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "ZEngine3D", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    {
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    {
        glewExperimental = GL_TRUE;
        glewInit();
    }

    {
        int pixelWidth, pixelHeight;
        glfwGetFramebufferSize(window, &pixelWidth, &pixelHeight);
        glViewport(0,0,pixelWidth, pixelHeight);
        glEnable(GL_DEPTH_TEST);
    }

    Shader lightingShader("./lighting.vs", "./lighting.fs");
    Shader lampShader("./lamp.vs", "./lamp.fs");

    GLfloat planeVertices[] = {
        // Positions          // Normals         // Texture Coords
         8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
        -8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,

         8.0f, -0.5f,  8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 0.0f,
        -8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  0.0f, 5.0f,
         8.0f, -0.5f, -8.0f,  0.0f, 1.0f, 0.0f,  5.0f, 5.0f
    };

    GLuint lightVBO, planeVBO;

    glGenBuffers(1, &lightVBO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    GLuint planeVAO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glBindVertexArray(0);

    GLuint floorTexture = loadTexture("../../assets/textures/metal.png");

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        update();
        Do_Movement();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.Use();
        GLint lightPosLoc    = glGetUniformLocation(lightingShader.Program, "light.position");
        GLint viewPosLoc     = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);

        glUniform1f(glGetUniformLocation(lightingShader.Program, "light.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "light.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "light.quadratic"), 0.032f);

        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)screenWidth/(GLfloat)screenHeight, 0.1f, 100.0f);
        glm::mat4 model;

        GLuint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLuint viewLoc  = glGetUniformLocation(lightingShader.Program, "view");
        GLuint projLoc  = glGetUniformLocation(lightingShader.Program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        lampShader.Use();
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc = glGetUniformLocation(lampShader.Program, "view");
        projLoc = glGetUniformLocation(lampShader.Program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        for (AmmoRound* shot = ammo; shot < ammo+ammoRounds; shot++) {
            if (shot->type != UNUSED) {
                model = glm::mat4();
                model = glm::translate(model, lightPos);
                model = glm::scale(model, glm::vec3(0.2f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            }
        }

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

GLuint loadTexture(GLchar* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

void fire() {
    AmmoRound* shot;
    for (shot = ammo; shot < ammo+ammoRounds; shot++)
        if (shot->type == UNUSED) break;

    if (shot >= ammo+ammoRounds) return;


    shot->particle.position = glm::vec3(0.0f, 1.5f, 0.0f);
    shot->startTime = glfwGetTime();
    shot->type = currentShotType;
    shot->particle.clearAccumulator();
}

void update(float dt) {
    assert(dt > 0.0f);

    for (AmmoRound* shot = ammo; shot < ammo+ammoRounds; shot++) {
        if (shot->type != UNUSED) {
           shot->particle.integrate(dt); 
           if (shot->particle.position.y < 0.0f ||
               shot->startTime+5000 < glfwGetTime() ||
               shot->particle.position.z > 200.0f) {
               shot->type = ShotType::UNUSED;
           }
        }
    }
}

void Do_Movement() {
    if (keys[GLFW_KEY_W]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (keys[GLFW_KEY_SPACE]) {
        camera.ProcessKeyboard(UP, deltaTime);
    }

    if (keys[GLFW_KEY_1] && !keysPressed[GLFW_KEY_1]) {
        currentShotType = ShotType::PISTOL;
        keysPressed[GLFW_KEY_1] = true;
    }
    if (keys[GLFW_KEY_2] && !keysPressed[GLFW_KEY_2]) {
        currentShotType = ShotType::ARTILLERY;
        keysPressed[GLFW_KEY_2] = true;
    }
    if (keys[GLFW_KEY_3] && !keysPressed[GLFW_KEY_3]) {
        currentShotType = ShotType::FIREBALL;
        keysPressed[GLFW_KEY_3] = true;
    }
    if (keys[GLFW_KEY_4] && !keysPressed[GLFW_KEY_4]) {
        currentShotType = ShotType::LASER;
        keysPressed[GLFW_KEY_4] = true;
    }
    if (keys[GLFW_KEY_SPACE] && !keysPressed[GLFW_KEY_SPACE]) {
        fire();
        keysPressed[GLFW_KEY_SPACE] = true;
    }
}

void error_callback(int errorCode, const char* description) {
    throw std::runtime_error(description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
            keysPressed[key] = false;
        }
        
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
       lastX = xpos; 
       lastY = ypos; 
       firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
   camera.ProcessMouseScroll(yoffset); 
}

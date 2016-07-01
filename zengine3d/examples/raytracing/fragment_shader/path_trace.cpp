#include <iostream>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint screenWidth(800), screenHeight(600);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void error_callback(int errorCode, const char* description);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void Do_Movement();

Camera camera(CameraStyle::NOCLIP, glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
bool keysPressed[1024];
GLfloat lastX(400), lastY(300);
bool firstMouse = true;

GLfloat deltaTime(0.0f);
GLfloat lastFrame(0.0f);

GLuint numSamples(1);

void RenderQuad();

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
        glGetError();
    }

    {
        int pixelWidth, pixelHeight;
        glfwGetFramebufferSize(window, &pixelWidth, &pixelHeight);
        glViewport(0,0,pixelWidth, pixelHeight);
    }

    Shader shader("./path_trace.vs", "./path_trace.fs");
    shader.Use();

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        Do_Movement();

        glClear(GL_COLOR_BUFFER_BIT);
        glUniform1i(glGetUniformLocation(shader.Program, "time"), (int)currentFrame);
        glUniform1i(glGetUniformLocation(shader.Program, "numSamples"), numSamples);
        glUniform1i(glGetUniformLocation(shader.Program, "screenWidth"), screenWidth);
        glUniform1i(glGetUniformLocation(shader.Program, "screenHeight"), screenHeight);

        RenderQuad();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad() {
    if (quadVBO == 0) {
	const GLfloat quadVertices[] = {
	    // Positions        // Texture Coords
	    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

    //if (keys[GLFW_KEY_G] && !keysPressed[GLFW_KEY_G]) {
        //enableGamma = !enableGamma;
        //std::cout << (enableGamma ? "gamma enabled" : "gamma disabled") << std::endl;
        //keysPressed[GLFW_KEY_G] = true;
    //}
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

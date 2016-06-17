#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

GLuint screenWidth(800), screenHeight(600);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void error_callback(int errorCode, const char* description);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement();
GLuint loadTexture(GLchar* path);
bool TestRayOBBIntersection(glm::vec3 ray_origin,
                            glm::vec3 ray_direction,
                            glm::vec3 aabb_min,
                            glm::vec3 aabb_max,
                            glm::mat4 ModelTransform,
                            float& intersection_distance);
struct Plane {
    glm::vec3 normal;
    float dot;
};

bool IntersectSegmentPlane(glm::vec3 a,
                           glm::vec3 b,
                           Plane p,
                           float &t, 
                           glm::vec3 &q);

Plane ComputePlane(glm::vec3 a, glm::vec3 b, glm::vec3 c);

void ScreenPosToWorldRay(int mouseX,
                         int mouseY,
                         int screenWidth,
                         int screenHeight,
                         glm::mat4 ViewMatrix,
                         glm::mat4 ProjectionMatrix,
                         glm::vec3& out_origin,
                         glm::vec3& out_direction);

//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90);
Camera camera(CameraStyle::RTS, glm::vec3(8.41f, 14.72f, -8.64f), glm::vec3(0.0f, 0.4f, 0.4f), -269, -41);
bool keys[1024];
GLfloat lastX(400), lastY(300);
bool firstMouse = true;

GLfloat deltaTime(0.0f);
GLfloat lastFrame(0.0f);

glm::vec3 clickIntersection(0.0f, 0.0f, 0.0f);       

glm::vec3 modelPosition(0.0f, 0.0f, 0.0f);
float modelRotation = 0;
float modelVelocity = 10;

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
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    {
        glewExperimental = GL_TRUE;
        glewInit();
        glGetError();
    }

    {
        int pixelWidth, pixelHeight;
        glfwGetFramebufferSize(window, &pixelWidth, &pixelHeight);
        glViewport(0,0,pixelWidth, pixelHeight);
        glEnable(GL_DEPTH_TEST);
    }

    Shader ourShader("./picking.vs", "./picking.fs");

    GLfloat planeVertices[] = {
         1.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        -1.0f,  0.0f, -1.0f, 0.0f, 1.0f,

         1.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  0.0f, -1.0f, 1.0f, 1.0f,
         1.0f,  0.0f, -1.0f, 1.0f, 1.0f
    };

    GLuint planeVAO, planeVBO;
    {
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glBindVertexArray(0);
    }

    const GLuint blackTexture = loadTexture("../../assets/textures/dark_gray.png");
    assert(glGetError() == GL_NO_ERROR);
    const GLuint whiteTexture = loadTexture("../../assets/textures/light_gray.png");
    assert(glGetError() == GL_NO_ERROR);
    const GLuint clickTexture = loadTexture("../../assets/textures/red.png");
    assert(glGetError() == GL_NO_ERROR);

    Model ourModel;
    assert(glGetError() == GL_NO_ERROR);
    ourModel.LoadMesh("../../assets/models/nanosuit/nanosuit.obj");
    assert(glGetError() == GL_NO_ERROR);

    Model bob;
    assert(glGetError() == GL_NO_ERROR);
    bob.LoadMesh("../../assets/models/boblampclean/boblampclean.md5mesh");
    assert(glGetError() == GL_NO_ERROR);
    Shader bobShader("../skeletal_animation/skeletal_animation.vs", "../skeletal_animation/skeletal_animation.fs");
    assert(glGetError() == GL_NO_ERROR);

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        Do_Movement();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        {
            glm::mat4 model;

            glm::vec3 difference = clickIntersection - modelPosition;
            glm::vec3 direction = glm::normalize(difference);
            // std::cout << "direction.x: " << direction.x << std::endl;
            // std::cout << "direction.y: " << direction.y << std::endl;
            // std::cout << "direction.z: " << direction.z << std::endl;
            // std::cout << "direction length: " << glm::length(direction) << std::endl;
            std::cout << "modelPosition.x: " << modelPosition.x << std::endl;
            std::cout << "modelPosition.y: " << modelPosition.y << std::endl;
            std::cout << "modelPosition.z: " << modelPosition.z << std::endl;

            std::cout << "clickIntersection.x: " << clickIntersection.x << std::endl;
            std::cout << "clickIntersection.y: " << clickIntersection.y << std::endl;
            std::cout << "clickIntersection.z: " << clickIntersection.z << std::endl;

            float angle = -glm::orientedAngle(direction, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelRotation = angle;
            // if (fabs(fmod(modelRotation,2*M_PI) - angle) > 0.7) {
            //     modelRotation += 0.5;
            // }

            std::cout << "angle: " << angle << std::endl;
            std::cout << "modelRotation: " << modelRotation << std::endl;

            if (glm::length(direction) > 0.0 && glm::length(difference) > 1.0) {
                modelPosition.x += direction.x * modelVelocity * deltaTime;
                modelPosition.z += direction.z * modelVelocity * deltaTime;
            }

            // model = glm::rotate(model, glm::dot(direction, glm::vec3(0,0,0)), glm::vec3(0.0f, 1.0f, 0.0f));
            // model = glm::lookat(modelPosition, 
            model = glm::translate(model, modelPosition);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            // model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, modelRotation, glm::vec3(0.0f, 1.0f, 0.0f));


            glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            ourModel.Render(ourShader);
        }


        //draw checkerboard
        int x(0), y(0), z(0);
        bool black = true;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                glBindVertexArray(planeVAO);
                if (black)
                    glBindTexture(GL_TEXTURE_2D, blackTexture);
                else
                    glBindTexture(GL_TEXTURE_2D, whiteTexture);

                glm::mat4 model;
                model = glm::translate(model, glm::vec3(x, y, z));
                glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
                x += 2;
                black = !black;
            }
            x = 0;
            z += 2;
            black = !black;
        }

        //draw click location
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, clickTexture);
        glm::mat4 model;
        model = glm::translate(model, clickIntersection);
        // model = glm::translate(model, glm::vec3(clickIntersection.x, clickIntersection.y, -clickIntersection.z));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        glBindTexture(GL_TEXTURE_2D, 0);

        {
            bobShader.Use();

            glm::mat4 projectionTransform = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
            glm::mat4 viewTransform = camera.GetViewMatrix();

            glUniformMatrix4fv(glGetUniformLocation(bobShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projectionTransform));
            glUniformMatrix4fv(glGetUniformLocation(bobShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(viewTransform));
            std::vector<glm::mat4> Transforms;

            bob.BoneTransform(currentFrame, Transforms);

            for (uint i = 0 ; i < Transforms.size() ; i++) {
                std::stringstream ss;
                ss
                    << "bones["
                    << i
                    << "]"
                    ;
                glUniformMatrix4fv(glGetUniformLocation(bobShader.Program, ss.str().c_str()), 1, GL_FALSE, glm::value_ptr(Transforms[i]));       
            }

            glm::mat4 modelTransform;
            modelTransform = glm::translate(modelTransform, glm::vec3(0.0f, 0.0f, 10.0f));
            modelTransform = glm::scale(modelTransform, glm::vec3(0.05f, 0.05f, 0.05f));
            modelTransform = glm::rotate(modelTransform, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            modelTransform = glm::rotate(modelTransform, 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(bobShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelTransform));

            bob.Render(bobShader);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}

GLuint loadTexture(GLchar* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
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
        }
        
    }

    // glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
    // glm::mat4 view = camera.GetViewMatrix();

    // glm::vec3 a = camera.Position;
    // glm::vec3 b = camera.Position + camera.Front * 100'000;

    // Plane p = ComputePlane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    // float t;
    // glm::vec3 q;
    // IntersectSegmentPlane(a, b, p, t, q);

    // clickIntersection = q;
    // clickIntersection.y += 0.01;
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


    // camera.ProcessMouseMovement(xoffset, yoffset);
    // std::cout << "camera position.x: " << camera.Position.x << std::endl;
    // std::cout << "camera position.y: " << camera.Position.y << std::endl;
    // std::cout << "camera position.z: " << camera.Position.z << std::endl;

    // std::cout << "camera up.x: " << camera.Up.x << std::endl;
    // std::cout << "camera up.y: " << camera.Up.y << std::endl;
    // std::cout << "camera up.z: " << camera.Up.z << std::endl;

    // std::cout << "camera yaw: " << camera.Yaw << std::endl;
    // std::cout << "camera pitch: " << camera.Pitch << std::endl;

        // glm::vec3 Position;
        // glm::vec3 Front;
        // glm::vec3 Up;
        // glm::vec3 Right;
        // glm::vec3 WorldUp;

        // GLfloat Yaw;
        // GLfloat Pitch;

        // GLfloat MovementSpeed;
        // GLfloat MouseSensitivity;
        // GLfloat Zoom;
}

void showClickLocation() {
    glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    glm::vec3 temp;
    glm::vec3 a = camera.Position;
    glm::vec3 b;
    // glm::vec3 b = camera.Position + camera.Front * 100'000;

    ScreenPosToWorldRay(lastX,
                        lastY,
                        screenWidth,
                        screenHeight,
                        view,
                        projection,
                        temp,
                        b);

    // std::cout << "camera.Front.x: " << camera.Front.x << std::endl;
    // std::cout << "camera.Front.y: " << camera.Front.y << std::endl;
    // std::cout << "camera.Front.z: " << camera.Front.z << std::endl;

    // std::cout << "a.x: " << a.x << std::endl;
    // std::cout << "a.y: " << a.y << std::endl;
    // std::cout << "a.z: " << a.z << std::endl;

    // std::cout << "b.x: " << b.x << std::endl;
    // std::cout << "b.y: " << b.y << std::endl;
    // std::cout << "b.z: " << b.z << std::endl;

    b *= 100'000;

    Plane p = ComputePlane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
    float t;
    glm::vec3 q;
    IntersectSegmentPlane(a, b, p, t, q);
    // std::cout << "intersection.x: " << q.x << std::endl;
    // std::cout << "intersection.y: " << q.y << std::endl;
    // std::cout << "intersection.z: " << q.z << std::endl;

    clickIntersection = q;
    clickIntersection.y += 0.01;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    showClickLocation();
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
   camera.ProcessMouseScroll(yoffset); 
   // std::cout << "camera zoom: " << camera.Zoom << std::endl;
}

bool TestRayOBBIntersection(glm::vec3 ray_origin,
                            glm::vec3 ray_direction,
                            glm::vec3 aabb_min,
                            glm::vec3 aabb_max,
                            glm::mat4 ModelTransform,
                            float& intersection_distance) {
    float tMin = 0.0f;
    float tMax = 100'000;

    glm::vec3 OBBposition_worldspace(ModelTransform[3].x,
                                     ModelTransform[3].y,
                                     ModelTransform[3].z);
    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    glm::vec3 xaxis(ModelTransform[0].x, ModelTransform[0].y, ModelTransform[0].z);
    float e = glm::dot(xaxis, delta);
    float f = glm::dot(ray_direction, xaxis);

    float t1 = (e+aabb_min.x)/f;
    float t2 = (e+aabb_max.x)/f;

    if (t1>t2) std::swap(t1, t2);
    if (t2 < tMax) tMax = t2;
    if (t1 > tMin) tMin = t1;
    if (tMax < tMin) return false;

    return true;
}

bool IntersectSegmentPlane(glm::vec3 a,
                           glm::vec3 b,
                           Plane p,
                           float &t, 
                           glm::vec3 &q) {
    glm::vec3 ab = b - a;
    t = (p.dot - glm::dot(p.normal, a)) / glm::dot(p.normal, ab);

    if (t >= 0.0f && t <= 1.0f) {
        q = a + t * ab;
        return true;
    }
    return false;
}

Plane ComputePlane(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    Plane p;
    p.normal = glm::normalize(glm::cross(b - a, c - a));
    p.dot = glm::dot(p.normal, a);
    return p;
}

void ScreenPosToWorldRay(int mouseX,
                         int mouseY,
                         int screenWidth,
                         int screenHeight,
                         glm::mat4 ViewMatrix,
                         glm::mat4 ProjectionMatrix,
                         glm::vec3& out_origin,
                         glm::vec3& out_direction) {
    mouseY = screenHeight - mouseY;

    glm::vec4 lRayStart_NDC(
            ((float)mouseX/(float)screenWidth - 0.5f) * 2.0f,
            ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f,
            -1.0f,
            1.0f);

    glm::vec4 lRayEnd_NDC(
            ((float)mouseX/(float)screenWidth - 0.5f) * 2.0f,
            ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f,
            0.0f,
            1.0f);

    glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);
    glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

    glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
    glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world  /= lRayStart_world.w;
    glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera   /= lRayEnd_camera.w;
    glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world    /= lRayEnd_world.w;

    glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
    lRayDir_world = glm::normalize(lRayDir_world);

    out_origin = glm::vec3(lRayStart_world);
    out_direction = glm::normalize(lRayDir_world);
}

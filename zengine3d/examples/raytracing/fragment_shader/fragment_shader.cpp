#include <iostream>
#include <fstream>

#include <cassert>

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"

#include <rt_camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void error_callback(int errorCode, const char* description);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void Do_Movement();

Camera camera(CameraStyle::NOCLIP, glm::vec3(0.0f, 1.0f, 5.0f));
bool keys[1024];
bool keysPressed[1024];
GLfloat lastX(400), lastY(300);
bool firstMouse = true;
bool didMove = false;
GLuint screenWidth(900), screenHeight(900);
//GLuint screenWidth(1600), screenHeight(1600);

GLfloat deltaTime(0.0f);
GLfloat lastFrame(0.0f);

GLuint numSamples(1);
GLuint textureID;

void RenderQuad();

struct gl_Material {
    glm::vec3 albedo;
    GLfloat fuzz;
    GLfloat ref_idx;
};

struct gl_Sphere {
    glm::vec3 center;
    GLfloat radius;
    gl_Material material;
};
void pass_sphere(gl_Sphere sphere, int index, Shader& shader) {
    {
        std::stringstream ss;
        ss << "spheres["
            << index
            << "]"
            << ".center";
        glUniform3fv(glGetUniformLocation(shader.Program, ss.str().c_str()), 1, glm::value_ptr(sphere.center));
    }
    {
        std::stringstream ss;
        ss << "spheres["
            << index
            << "]"
            << ".radius";
        glUniform1f(glGetUniformLocation(shader.Program, ss.str().c_str()), sphere.radius);
    }
    {
        std::stringstream ss;
        ss << "spheres["
            << index
            << "]"
            << ".material"
            << ".albedo";
        glUniform3fv(glGetUniformLocation(shader.Program, ss.str().c_str()), 1, glm::value_ptr(sphere.material.albedo));
    }
    {
        std::stringstream ss;
        ss << "spheres["
            << index
            << "]"
            << ".material"
            << ".fuzz";
        glUniform1f(glGetUniformLocation(shader.Program, ss.str().c_str()), sphere.material.fuzz);
    }
    {
        std::stringstream ss;
        ss << "spheres["
            << index
            << "]"
            << ".material"
            << ".ref_idx";
        glUniform1f(glGetUniformLocation(shader.Program, ss.str().c_str()), sphere.material.ref_idx);
    }
}

//void (Sphere* l, int n, float time0, float time1) {
    //int axis = int(3*drand48());
    //if (axis == 0) {
        //qsort(l, n, sizeof(hitable*), box_x_compare);
    //}
    //else if (axis == 1) {
        //qsort(l, n, sizeof(hitable*), box_y_compare);
    //}
    //else {
        //qsort(l, n, sizeof(hitable*), box_z_compare);
    //}
    //if (n == 1) {
        //left = right = l[0];
    //}
    //else if (n == 2) {
        //left = l[0];
        //right = l[1];
    //}
    //else {
        //left = new bvh_node(l, n/2, time0, time1);
        //right = new bvh_node(l + n/2, n - n/2, time0, time1);
    //}

    //aabb box_left, box_right;
    //if (   !left->bounding_box(time0, time1, box_left)
        //|| !right->bounding_box(time0, time1, box_right)) {
        //throw std::logic_error("no bounding box in bvh_node construction");
    //}
    //box = aabb::surrounding_box(box_left, box_right);
//}

int main(int argc, const char *argv[]) {

    if (argc == 3) {
        screenWidth = screenHeight = std::stoi(argv[1]);
        numSamples = std::stoi(argv[2]);
    }

    {
        glfwInit();
        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 1);
    }

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "ZEngine3D", nullptr, nullptr);
    //const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    //screenWidth = mode->width;
    //screenHeight = mode->height;
    //GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "ZEngine3D", glfwGetPrimaryMonitor(), nullptr);
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

    //gl_Sphere red =   gl_Sphere{glm::vec3(0.0f, 4.0f, -3.0f),     4.0f,    gl_Material{glm::vec3(0.7f, 0.4f, 0.1f), 1.0f, 1.47f}};
    gl_Sphere red =   gl_Sphere{glm::vec3(0.0f, 4.0f, -3.0f),     4.0f,    gl_Material{glm::vec3(0.1f, 0.1f, 0.9f), 1.0f, 1.33f}};
    gl_Sphere green = gl_Sphere{glm::vec3(8.0f, 3.0f, -3.0f),     3.0f,    gl_Material{glm::vec3(0.2f, 0.6f, 0.2f), 0.1f, 0.0f }};
    gl_Sphere blue =  gl_Sphere{glm::vec3(-6.0f, 2.0f, -3.0f),    2.0f,    gl_Material{glm::vec3(0.2f, 0.2f, 0.6f), 1.0f, 0.0f }};
    gl_Sphere big =   gl_Sphere{glm::vec3(0.0f, -1000.0f, -3.0f), 1000.0f, gl_Material{glm::vec3(0.5f, 0.5f, 0.5f), 1.0f, 0.0f }};
    gl_Sphere spheres[4] = {red, blue, green, big};
    for (int i = 0; i < 4; i++) {
        pass_sphere(spheres[i], i, shader);
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwSwapInterval(1);

    int nbFrames = 0;
    GLfloat lastMeasured = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        ++nbFrames;
        if (currentFrame - lastMeasured >= 1.0f) {
            std::cout << 1000.0f/(float)nbFrames << "ms/frame" << std::endl;
            nbFrames = 0;
            lastMeasured += 1.0f;
        }

        glfwPollEvents();
        Do_Movement();

        glClear(GL_COLOR_BUFFER_BIT);

        glUniform2f(glGetUniformLocation(shader.Program, "resolution"), screenWidth, screenHeight);

        glm::vec3 lookfrom = camera.Position;
        glm::vec3 lookat = camera.Position + camera.Front;
        glm::vec3 vup = camera.Up;
        float vfov = camera.Zoom;
        float aperture = 0.0f;
        float aspect = (float)screenWidth / (float)screenHeight;
        float dist_to_focus = glm::length(lookfrom - lookat);

        rt_camera cam(lookfrom, lookat, vup, vfov, aspect, aperture, dist_to_focus);
        glUniform3f(glGetUniformLocation(shader.Program, "camera.origin"), cam.origin.x, cam.origin.y, cam.origin.z);
        glUniform3f(glGetUniformLocation(shader.Program, "camera.lower_left_corner"), cam.lower_left_corner.x, cam.lower_left_corner.y, cam.lower_left_corner.z);
        glUniform3f(glGetUniformLocation(shader.Program, "camera.horizontal"), cam.horizontal.x, cam.horizontal.y, cam.horizontal.z);
        glUniform3f(glGetUniformLocation(shader.Program, "camera.vertical"), cam.vertical.x, cam.vertical.y, cam.vertical.z);
        glUniform3f(glGetUniformLocation(shader.Program, "camera.u"), cam.u.x, cam.u.y, cam.u.z);
        glUniform3f(glGetUniformLocation(shader.Program, "camera.v"), cam.v.x, cam.v.y, cam.v.z);
        glUniform1f(glGetUniformLocation(shader.Program, "camera.lens_radius"), cam.lens_radius);

        int seed = (unsigned int)(INT_MAX*currentFrame);
        glUniform1i(glGetUniformLocation(shader.Program, "time"), seed);


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
	    -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	    1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	    1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
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
    didMove = true;
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

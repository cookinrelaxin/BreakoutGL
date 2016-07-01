#include <iostream>
#include <fstream>

#include <cassert>


#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <OpenCL/opencl.h>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_CPU
//#define DEVICE CL_DEVICE_TYPE_GPU
#endif /* ifndef DEVICE */

//#define DEBUG_CL

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
GLuint screenWidth(300), screenHeight(300);
//GLuint screenWidth(900), screenHeight(900);

GLfloat deltaTime(0.0f);
GLfloat lastFrame(0.0f);

GLuint numSamples(500);
//const float GAMMA = 2.2f;
const int NUM_BOUNCES(100);
GLuint textureID;

void RenderQuad();

inline void checkErr(cl_int err, const char* name) {
    std::string errString;
    switch (err) {
        case CL_INVALID_CONTEXT:
            errString = "CL_INVALID_CONTEXT";
            break;
        case CL_INVALID_VALUE:
            errString = "CL_INVALID_VALUE";
            break;
        case CL_INVALID_BUFFER_SIZE:
            errString = "CL_INVALID_BUFFER_SIZE";
            break;
        case CL_INVALID_HOST_PTR:
            errString = "CL_INVALID_HOST_PTR";
            break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            errString = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
            break;
        case CL_OUT_OF_RESOURCES:
            errString = "CL_OUT_OF_RESOURCES";
            break;
        case CL_OUT_OF_HOST_MEMORY:
            errString = "CL_OUT_OF_HOST_MEMORY";
            break;

        //clBuildProgram
        case CL_INVALID_PROGRAM:
            errString = "CL_INVALID_PROGRAM";
            break;
        case CL_INVALID_DEVICE:
            errString = "CL_INVALID_DEVICE";
            break;
        case CL_INVALID_BINARY:
            errString = "CL_INVALID_BINARY";
            break;
        case CL_INVALID_BUILD_OPTIONS:
            errString = "CL_INVALID_BUILD_OPTIONS";
            break;
        case CL_INVALID_OPERATION:
            errString = "CL_INVALID_OPERATION";
            break;
        case CL_COMPILER_NOT_AVAILABLE:
            errString = "CL_COMPILER_NOT_AVAILABLE";
            break;
        case CL_BUILD_PROGRAM_FAILURE:
            errString = "CL_BUILD_PROGRAM_FAILURE";
            break;

        case CL_SUCCESS:
            errString = "CL_SUCCESS";
            break;
    }
    if (err != CL_SUCCESS)
        throw std::runtime_error("ERROR: " + std::string(name) + " (" + errString + ")");
#ifdef DEBUG_CL
    std::cout << "ERROR: " + std::string(name) + " (" + errString + ")" << std::endl;
#endif
}

std::string read_program(std::string name) {
    std::ifstream file(name);
    std::string str;
    std::string file_contents;
    while (std::getline(file, str)) {
        file_contents += str;
        file_contents.push_back('\n');
    }
    return file_contents;
}

struct cl_Material {
    cl_float3 albedo;
    cl_float fuzz;
};

struct cl_Sphere {
    cl_float3 center;
    cl_float radius;
    cl_Material material;
};

struct cl_Camera {
        cl_float3 origin;
        cl_float3 lower_left_corner;
        cl_float3 horizontal;
        cl_float3 vertical;
        cl_float3 u,v,w;
        cl_float lens_radius;
};

struct cl_Camera initCamera(glm::vec3 lookfrom,
                            glm::vec3 lookat,
                            glm::vec3 vup,
                            float vfov,
                            float aspect,
                            float aperture,
                            float focus_dist) {
    struct cl_Camera c;
    c.lens_radius = (cl_float){aperture / 2.0f};

    const float theta = vfov * (float)M_PI/180.0f; 
    const float half_height = tanf(theta/2.0f);
    const float half_width = aspect * half_height;

    c.origin = (cl_float3){lookfrom.x, lookfrom.y, lookfrom.z};

    glm::vec3 w = glm::normalize(lookfrom - lookat);
    c.w = (cl_float3){w.x, w.y, w.z};

    glm::vec3 u = glm::normalize(glm::cross(vup, w));
    c.u = (cl_float3){u.x, u.y, u.z};

    glm::vec3 v = glm::cross(w, u);
    c.v = (cl_float3){v.x, v.y, v.z};

    glm::vec3 l = lookfrom - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
    c.lower_left_corner = (cl_float3){l.x, l.y, l.z};

    glm::vec3 horizontal = 2.0f * half_width * focus_dist * u;
    c.horizontal = (cl_float3){horizontal.x, horizontal.y, horizontal.z};

    glm::vec3 vertical = 2.0f * half_height * focus_dist * v;
    c.vertical = (cl_float3){vertical.x, vertical.y, vertical.z};

    return c;
}

int main(int argc, const char *argv[]) {

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

    //~~~~ OpenCL

    cl_int err;

    std::string program_str = read_program("path_trace.cl");
    const char* program_cstr = program_str.c_str();

    const int pixelCount = screenWidth * screenHeight;
    const int pixelBufferLength = 3 * pixelCount;

    float* h_colors = new float[pixelBufferLength];
    int* h_seeds = new int[pixelCount];
    time_t t;
    srand((unsigned) time(&t));
    for (int i = 0; i < pixelCount; i++) {
       h_seeds[i] = rand(); 
    }

    const unsigned int numSpheres = 5;
    cl_Sphere* h_spheres = new cl_Sphere[numSpheres];

    h_spheres[0] = cl_Sphere{.center = cl_float3{0.0f, 1.0f, 0.0f},
                          .radius = cl_float{1.0f},
                          .material = cl_Material{cl_float3{0.9f, 0.1f, 0.1f}, cl_float(1.0f)}}; 
    h_spheres[1] = cl_Sphere{.center = cl_float3{0.0f, -1000.0f, 0.0f},
                          .radius = cl_float{1000.0f},
                          .material = cl_Material{cl_float3{0.5f, 0.5f, 0.5f}, cl_float(1.0f)}}; 
                          //.material = cl_Material{cl_float3{0.5f, 0.5f, 0.5f}, cl_float(0.0f)}}; 
    h_spheres[2] = cl_Sphere{.center = cl_float3{-3.0f, 0.8f, 0.0f},
                          .radius = cl_float{0.8f},
                          .material = cl_Material{cl_float3{0.4f, 0.9f, 0.2f}, cl_float(0.01f)}}; 
    h_spheres[3] = cl_Sphere{.center = cl_float3{-2.0f, 0.1f, 1.0f},
                          .radius = cl_float{0.1f},
                          .material = cl_Material{cl_float3{0.1f, 0.1f, 0.9f}, cl_float(0.1f)}}; 
    h_spheres[4] = cl_Sphere{.center = cl_float3{2.1f, 0.6f, -2.0f},
                          .radius = cl_float{0.6f},
                          .material = cl_Material{cl_float3{212.0f/255.0f, 175.0f/255.0f, 55.0f/255.0f}, cl_float(0.5f)}}; 

    size_t global;

    cl_device_id device_id;
    cl_context context;
    cl_command_queue commands;
    cl_program program;
    cl_kernel ko_color_pixel;

    cl_mem d_colors;
    cl_mem d_seeds;
    cl_mem d_spheres;

    cl_uint num_platforms;
    err = clGetPlatformIDs(0, nullptr, &num_platforms);
    checkErr(err, "Finding platforms");
    if (num_platforms == 0) {
        throw std::runtime_error("Found 0 platforms!");
    }

    cl_platform_id platform[num_platforms];
    err = clGetPlatformIDs(num_platforms, platform, nullptr);
    checkErr(err, "Getting platforms");
    //std::cout << "Number of OpenCL platforms: " << num_platforms << std::endl;

    for (int i = 0; i < num_platforms; i++) {
       err = clGetDeviceIDs(platform[i], DEVICE, 1, &device_id, nullptr);
       if (err == CL_SUCCESS)
           break;
    }
    if (err != CL_SUCCESS)
        throw std::runtime_error("no viable platforms found");

    context = clCreateContext(0, 1, &device_id, nullptr, nullptr, &err);
    checkErr(err, "creating context");

    commands = clCreateCommandQueue(context, device_id, 0, &err);
    checkErr(err, "creating command queue");

    program = clCreateProgramWithSource(context, 1, (const char**)& program_cstr, nullptr, &err);
    checkErr(err, "creating program");

    //const char options[] = "-cl-std=CL1.0";
    //const char options[] = "-Werror";
    //const char options[] = "";
    const char options[] = "-cl-fast-relaxed-math";
    err = clBuildProgram(program, 1, &device_id, options, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t len;

        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);
        char* log = new char[len];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(log), log, NULL);
        std::cout << log << std::endl;

        checkErr(err, "building program");
        delete[] log;
    }

    ko_color_pixel = clCreateKernel(program, "color_pixel", &err);
    checkErr(err, "Creating Kernel");

    d_colors = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * pixelBufferLength, nullptr, &err);
    checkErr(err, "creating buffer d_colors");

    d_seeds = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(int) * pixelCount, (void*)h_seeds, &err);
    checkErr(err, "creating buffer d_seeds");

    d_spheres = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(cl_Sphere) * numSpheres, (void*)h_spheres, &err);
    checkErr(err, "creating buffer d_spheres");

    err = clEnqueueWriteBuffer(commands, d_seeds, CL_TRUE, 0, sizeof(int) * pixelCount, h_seeds, 0, nullptr, nullptr);
    checkErr(err, "Copying h_seeds to device at d_seeds");

    err = clEnqueueWriteBuffer(commands, d_spheres, CL_TRUE, 0, sizeof(cl_Sphere) * numSpheres, h_spheres, 0, nullptr, nullptr);
    checkErr(err, "Copying h_spheres to device at d_spheres");

    glGenTextures(1, &textureID);
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

        glm::vec3 lookfrom = camera.Position;
        glm::vec3 lookat = camera.Position + camera.Front;
        glm::vec3 vup = camera.Up;
        float vfov = camera.Zoom;
        float aperture = 0.0f;
        float aspect = (float)screenWidth / (float)screenHeight;
        float dist_to_focus = glm::length(lookfrom - lookat);

        cl_Camera cam = initCamera(lookfrom, lookat, vup, vfov, aspect, aperture, dist_to_focus); 

        //h_spheres[0] = Sphere{.center = cl_float3{0.0f, 1.0f, 0.0f},
            //.radius = cl_float{1.0f},
            //.material = Material{cl_float3{0.9f, 0.1f, 0.1f}}}; 

        h_spheres[0].center = cl_float3{sinf(currentFrame)/2.0f, 1.0f, 0.0f};
        //h_spheres[0].radius = cl_float{fabsf(sinf(currentFrame))};

        int seed = (unsigned int)(INT_MAX*currentFrame);
        //std::cout << "seed: " << seed << std::endl;
        //std::cout << "currentFrame: " << currentFrame << std::endl;
        //return 0;

        err  = clSetKernelArg(ko_color_pixel, 0, sizeof(cl_mem), &d_colors);
        err  = clSetKernelArg(ko_color_pixel, 1, sizeof(cl_mem), &d_seeds);
        err |= clSetKernelArg(ko_color_pixel, 2, sizeof(int), &seed);
        err |= clSetKernelArg(ko_color_pixel, 3, sizeof(cl_mem), &d_spheres);
        err |= clSetKernelArg(ko_color_pixel, 4, sizeof(unsigned int), &numSpheres);
        err |= clSetKernelArg(ko_color_pixel, 5, sizeof(unsigned int), &pixelCount);
        err |= clSetKernelArg(ko_color_pixel, 6, sizeof(unsigned int), &numSamples);
        err |= clSetKernelArg(ko_color_pixel, 7, sizeof(unsigned int), &screenWidth);
        err |= clSetKernelArg(ko_color_pixel, 8, sizeof(unsigned int), &screenHeight);
        err |= clSetKernelArg(ko_color_pixel, 9, sizeof(cl_Camera), &cam);
        checkErr(err, "setting kernel arguments");

        global = pixelCount;

        err = clEnqueueNDRangeKernel(commands, ko_color_pixel, 1, nullptr, &global, nullptr, 0, nullptr, nullptr);
        checkErr(err, "enqueuing kernel");

        err = clFinish(commands);
        checkErr(err, "waiting for the kernel to finish");

        err = clEnqueueReadBuffer(commands, d_colors, CL_TRUE, 0, sizeof(float) * pixelBufferLength, h_colors, 0, nullptr, nullptr);
        checkErr(err, "reading back d_colors");

        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, textureID);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, h_colors);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        RenderQuad();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    // clean up
    {
        clReleaseMemObject(d_colors);
        clReleaseMemObject(d_seeds);
        clReleaseProgram(program);
        clReleaseKernel(ko_color_pixel);
        clReleaseCommandQueue(commands);
        clReleaseContext(context);

        delete[] h_colors;
    }

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

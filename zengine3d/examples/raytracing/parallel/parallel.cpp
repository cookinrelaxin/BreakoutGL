#include <iostream>
#include <fstream>

#include <cassert>

#include <OpenCL/opencl.h>

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_CPU
//#define DEVICE CL_DEVICE_TYPE_GPU
#endif /* ifndef DEVICE */

//#define DEBUG_CL

const int NUM_BOUNCES(50.0f);

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

struct Lambertian {
    cl_float3 albedo;
};

struct Sphere {
    cl_float3 center;
    cl_float radius;
    Lambertian material;
};

int main(int argc, const char *argv[]) {
    const float gamma = 2.2f;
    const int screenWidth = 1200;
    const int screenHeight = 600;
    const int numSamples = 10;

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
    Sphere* h_spheres = new Sphere[numSpheres];

    h_spheres[0] = Sphere{.center = cl_float3{0.0f, 1.0f, 0.0f},
                          .radius = cl_float{1.0f},
                          .material = Lambertian{cl_float3{0.9f, 0.1f, 0.1f}}}; 
    h_spheres[1] = Sphere{.center = cl_float3{0.0f, -1000.0f, 0.0f},
                          .radius = cl_float{1000.0f},
                          .material = Lambertian{cl_float3{0.5f, 0.5f, 0.5f}}}; 
    h_spheres[2] = Sphere{.center = cl_float3{-3.0f, 0.8f, 0.0f},
                          .radius = cl_float{0.8f},
                          .material = Lambertian{cl_float3{0.1f, 0.9f, 0.1f}}}; 
    h_spheres[3] = Sphere{.center = cl_float3{-2.0f, 0.1f, 1.0f},
                          .radius = cl_float{0.1f},
                          .material = Lambertian{cl_float3{0.1f, 0.1f, 0.9f}}}; 
    h_spheres[4] = Sphere{.center = cl_float3{1.8f, 0.6f, 0.0f},
                          .radius = cl_float{0.6f},
                          .material = Lambertian{cl_float3{1.0f, 1.0f, 1.0f}}}; 

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
    const char options[] = "";
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

    d_spheres = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(Sphere) * numSpheres, (void*)h_spheres, &err);
    checkErr(err, "creating buffer d_spheres");

    err = clEnqueueWriteBuffer(commands, d_seeds, CL_TRUE, 0, sizeof(int) * pixelCount, h_seeds, 0, nullptr, nullptr);
    checkErr(err, "Copying h_seeds to device at d_seeds");

    err = clEnqueueWriteBuffer(commands, d_spheres, CL_TRUE, 0, sizeof(Sphere) * numSpheres, h_spheres, 0, nullptr, nullptr);
    checkErr(err, "Copying h_spheres to device at d_spheres");

    err  = clSetKernelArg(ko_color_pixel, 0, sizeof(cl_mem), &d_colors);
    err |= clSetKernelArg(ko_color_pixel, 1, sizeof(cl_mem), &d_seeds);
    err |= clSetKernelArg(ko_color_pixel, 2, sizeof(cl_mem), &d_spheres);
    err |= clSetKernelArg(ko_color_pixel, 3, sizeof(unsigned int), &numSpheres);
    err |= clSetKernelArg(ko_color_pixel, 4, sizeof(unsigned int), &pixelCount);
    err |= clSetKernelArg(ko_color_pixel, 5, sizeof(unsigned int), &numSamples);
    err |= clSetKernelArg(ko_color_pixel, 6, sizeof(unsigned int), &screenWidth);
    err |= clSetKernelArg(ko_color_pixel, 7, sizeof(unsigned int), &screenHeight);
    checkErr(err, "setting kernel arguments");

    ////std::chrono::time_point<std::chrono::system_clock> start, end;
    ////start = std::chrono::system_clock::now();

    global = pixelCount;

    err = clEnqueueNDRangeKernel(commands, ko_color_pixel, 1, nullptr, &global, nullptr, 0, nullptr, nullptr);
    checkErr(err, "enqueuing kernel 1st time");

    err = clFinish(commands);
    checkErr(err, "waiting for the kernel to finish");

    err = clEnqueueReadBuffer(commands, d_colors, CL_TRUE, 0, sizeof(float) * pixelBufferLength, h_colors, 0, nullptr, nullptr);
    checkErr(err, "reading back d_colors");

    std::cout << "P3\n" << screenWidth << " " << screenHeight << "\n255" << std::endl;
    for (int i = 0; i < pixelBufferLength; i+=3) {
        int ir = int(255.99*h_colors[i]);
        int ig = int(255.99*h_colors[i+1]);
        int ib = int(255.99*h_colors[i+2]);

        std::cout << ir << " " << ig << " " << ib << std::endl;
    }

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

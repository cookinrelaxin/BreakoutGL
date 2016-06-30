#include <utility>
#include <OpenCL/opencl.h>

#include <iostream>

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_CPU
#endif /* ifndef DEVICE */

extern double wtime();
extern int output_device_info(cl_device_id);

inline void checkErr(cl_int err, const char* name) {
    if (err != CL_SUCCESS)
        throw std::runtime_error("ERROR: " + std::string(name) + " (" + std::to_string(err) + ")");
}

const float TOL(0.001f);
const unsigned int LENGTH(100'0);

const char* KernelSource = "\n" \
"__kernel void vadd(                            \n" \
"       __global float* a,                      \n" \
"       __global float* b,                      \n" \
"       __global float* c,                      \n" \
"       const unsigned int count)               \n" \
"{                                              \n" \
"       int i = get_global_id(0);               \n" \
"       if (i < count)                          \n" \
"               c[i] = a[i] + b[i];             \n" \
"}                                              \n" \
"\n";

int main(int argc, const char *argv[]) {
    cl_int err;

    float* h_a = (float*) calloc(LENGTH, sizeof(float));
    float* h_b = (float*) calloc(LENGTH, sizeof(float));
    float* h_c = (float*) calloc(LENGTH, sizeof(float));

    unsigned int correct;

    size_t global;

    cl_device_id device_id;
    cl_context context;
    cl_command_queue commands;
    cl_program program;
    cl_kernel ko_vadd;

    cl_mem d_a;
    cl_mem d_b;
    cl_mem d_c;

    int i = 0;
    int count = LENGTH;
    for (int i = 0; i < count; i++) {
        h_a[i] = rand() / (float)RAND_MAX;
        h_b[i] = rand() / (float)RAND_MAX;
    }

    cl_uint num_platforms;
    err = clGetPlatformIDs(0, nullptr, &num_platforms);
    checkErr(err, "Finding platforms");
    if (num_platforms == 0) {
        throw std::runtime_error("Found 0 platforms!");
    }

    cl_platform_id platform[num_platforms];
    err = clGetPlatformIDs(num_platforms, platform, nullptr);
    checkErr(err, "Getting platforms");
    std::cout << "Number of OpenCL platforms: " << num_platforms << std::endl;

    for (int i = 0; i < num_platforms; i++) {
       err = clGetDeviceIDs(platform[i], DEVICE, 1, &device_id, nullptr);
       if (err == CL_SUCCESS)
           break;
    }

    context = clCreateContext(0, 1, &device_id, nullptr, nullptr, &err);
    checkErr(err, "creating context");

    commands = clCreateCommandQueue(context, device_id, 0, &err);
    checkErr(err, "creating command queue");

    program = clCreateProgramWithSource(context, 1, (const char**)& KernelSource, nullptr, &err);
    checkErr(err, "creating program");

    err = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        std::cout << buffer << std::endl;

        throw std::runtime_error("Error: Failed to build program executable!");
    }

    ko_vadd = clCreateKernel(program, "vadd", &err);
    checkErr(err, "Creating Kernel");

    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * count, nullptr, &err);
    checkErr(err, "creating buffer d_a");

    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * count, nullptr, &err);
    checkErr(err, "creating buffer d_b");

    d_c = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * count, nullptr, &err);
    checkErr(err, "creating buffer d_c");

    err = clEnqueueWriteBuffer(commands, d_a, CL_TRUE, 0, sizeof(float) * count, h_a, 0, nullptr, nullptr);
    checkErr(err, "Copying h_a to device at d_a");

    err = clEnqueueWriteBuffer(commands, d_b, CL_TRUE, 0, sizeof(float) * count, h_b, 0, nullptr, nullptr);
    checkErr(err, "Copying h_b to device at d_b");

    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(ko_vadd, 3, sizeof(unsigned int), &count);
    checkErr(err, "setting kernel arguments");

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    global = count;
    err = clEnqueueNDRangeKernel(commands, ko_vadd, 1, nullptr, &global, nullptr, 0, nullptr, nullptr);
    checkErr(err, "enqueuing kernel");

    err = clFinish(commands);
    checkErr(err, "waiting for kernel to finish");

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "The kernel ran in " << elapsed_seconds.count() << " seconds." << std::endl;

    err = clEnqueueReadBuffer(commands, d_c, CL_TRUE, 0, sizeof(float) * count, h_c, 0, nullptr, nullptr);
    checkErr(err, "read output array");

    start = std::chrono::system_clock::now();

    float tmp = 0.0f;
    for (int i = 0; i < count; i++) {
        tmp = h_a[i] + h_b[i];
    }

    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::cout << "The sequential algorithm ran in " << elapsed_seconds.count() << " seconds." << std::endl;


    correct = 0;
    tmp = 0.0f;

    for (int i = 0; i < count; i++) {
       tmp = h_a[i] + h_b[i]; 
       tmp -= h_c[i];
       if (tmp*tmp < TOL*TOL) {
          ++correct; 
       }
       else {
           std::cout
               << "tmp: " << std::to_string(tmp)
               << "h_a: " << std::to_string(h_a[i])
               << "h_b: " << std::to_string(h_b[i])
               << "h_c: " << std::to_string(h_c[i])
               << std::endl;
       }
    }

    std::cout
        << "C = A+B: "
        << std::to_string(correct)
        << " out of "
        << count
        << " results were correct."
        << std::endl;

    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(ko_vadd);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    free(h_a);
    free(h_b);
    free(h_c);

    return 0;
}

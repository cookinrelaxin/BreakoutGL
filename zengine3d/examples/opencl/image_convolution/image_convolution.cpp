#include <utility>
#include <OpenCL/opencl.h>

#include <iostream>

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
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
    float* h_d = (float*) calloc(LENGTH, sizeof(float));
    float* h_e = (float*) calloc(LENGTH, sizeof(float));
    float* h_f = (float*) calloc(LENGTH, sizeof(float));
    float* h_g = (float*) calloc(LENGTH, sizeof(float));
    float* h_h = (float*) calloc(LENGTH, sizeof(float));

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
    cl_mem d_d;
    cl_mem d_e;
    cl_mem d_f;
    cl_mem d_g;

    int i = 0;
    int count = LENGTH;
    for (int i = 0; i < count; i++) {
        h_a[i] = rand() / (float)RAND_MAX;
        h_b[i] = rand() / (float)RAND_MAX;
        h_e[i] = rand() / (float)RAND_MAX;
        h_g[i] = rand() / (float)RAND_MAX;
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

    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * count, h_a, &err);
    checkErr(err, "creating buffer d_a");

    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * count, h_b, &err);
    checkErr(err, "creating buffer d_b");

    d_e = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * count, h_e, &err);
    checkErr(err, "creating buffer d_e");

    d_g = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * count, h_g, &err);
    checkErr(err, "creating buffer d_g");

    d_c = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * count, nullptr, &err);
    checkErr(err, "creating buffer d_c");

    d_d = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * count, nullptr, &err);
    checkErr(err, "creating buffer d_d");

    d_f = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * count, nullptr, &err);
    checkErr(err, "creating buffer d_f");

    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(ko_vadd, 3, sizeof(unsigned int), &count);
    checkErr(err, "setting kernel arguments");

    //std::chrono::time_point<std::chrono::system_clock> start, end;
    //start = std::chrono::system_clock::now();

    global = count;
    err = clEnqueueNDRangeKernel(commands, ko_vadd, 1, nullptr, &global, nullptr, 0, nullptr, nullptr);
    checkErr(err, "enqueuing kernel 1st time");

    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_e);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_d);
    checkErr(err, "setting kernel arguments");

    err = clEnqueueNDRangeKernel(commands, ko_vadd, 1, nullptr, &global, nullptr, 0, nullptr, nullptr);
    checkErr(err, "enqueuing kernel 2nd time");

    err  = clSetKernelArg(ko_vadd, 0, sizeof(cl_mem), &d_g);
    err |= clSetKernelArg(ko_vadd, 1, sizeof(cl_mem), &d_d);
    err |= clSetKernelArg(ko_vadd, 2, sizeof(cl_mem), &d_f);
    checkErr(err, "setting kernel arguments");

    err = clEnqueueNDRangeKernel(commands, ko_vadd, 1, nullptr, &global, nullptr, 0, nullptr, nullptr);
    checkErr(err, "enqueuing kernel 3rd time");

    err = clEnqueueReadBuffer(commands, d_f, CL_TRUE, 0, sizeof(float) * count, h_f, 0, nullptr, nullptr);
    checkErr(err, "reading back d_f");

    correct = 0;
    float tmp = 0.0f;

    for (int i = 0; i < count; i++) {
       tmp = h_a[i] + h_b[i] + h_e[i] + h_g[i]; 
       tmp -= h_f[i];
       if (tmp*tmp < TOL*TOL) {
          ++correct; 
       }
       else {
           std::cout
               << "tmp: " << std::to_string(tmp)
               << "h_a: " << std::to_string(h_a[i])
               << "h_b: " << std::to_string(h_b[i])
               << "h_e: " << std::to_string(h_e[i])
               << "h_g: " << std::to_string(h_g[i])
               << "h_f: " << std::to_string(h_f[i])
               << std::endl;
       }
    }

    std::cout
        << "C = A+B+E+G: "
        << std::to_string(correct)
        << " out of "
        << count
        << " results were correct."
        << std::endl;

    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseMemObject(d_d);
    clReleaseMemObject(d_e);
    clReleaseMemObject(d_f);
    clReleaseMemObject(d_g);

    clReleaseProgram(program);
    clReleaseKernel(ko_vadd);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);

    free(h_a);
    free(h_b);
    free(h_c);
    free(h_d);
    free(h_e);
    free(h_f);
    free(h_g);

    return 0;
}

#include <utility>
#include <OpenCL/opencl.h>

#include <iostream>

//#define DEVICE CL_DEVICE_TYPE_GPU

const std::string hw("Hello World\n");

inline void checkErr(cl_int err, const char* name) {
    if (err != CL_SUCCESS)
        throw std::runtime_error("ERROR: " + std::string(name) + " (" + std::to_string(err) + ")");
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "please specify gpu or cpu" << std::endl;
        return 1;
    }
    std::string d = argv[1];

    cl_device_type DEVICE = (d == "gpu") ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU;
    cl_int err;
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
       cl_char str[10240] = {0}; 

       err = clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, sizeof(str), &str, nullptr);
       checkErr(err, "Getting platform name");
       std::cout << "Platform: " << str << std::endl;

       err = clGetPlatformInfo(platform[i], CL_PLATFORM_VENDOR, sizeof(str), &str, nullptr);
       checkErr(err, "Getting platform vendor");
       std::cout << "Vendor: " << str << std::endl;

       err = clGetPlatformInfo(platform[i], CL_PLATFORM_VERSION, sizeof(str), &str, nullptr);
       checkErr(err, "Getting platform OpenCL version");
       std::cout << "Version: " << str << std::endl;

       cl_uint num_devices;
       err = clGetDeviceIDs(platform[i], DEVICE, 0, nullptr, &num_devices);
       checkErr(err, "Finding devices");

       cl_device_id device[num_devices];
       err = clGetDeviceIDs(platform[i], DEVICE, num_devices, device, nullptr);
       checkErr(err, "Getting devices");
       std::cout << "Number of devices: " << num_devices << std::endl;

       for (int j = 0; j < num_devices; j++) {
           std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

           err = clGetDeviceInfo(device[i], CL_DEVICE_NAME, sizeof(str), &str, nullptr);
           checkErr(err, "Getting device name");
           std::cout << "Name: " << str << std::endl;

           err = clGetDeviceInfo(device[i], CL_DEVICE_OPENCL_C_VERSION, sizeof(str), &str, nullptr);
           checkErr(err, "Getting device OpenCL C version");
           std::cout << "Version: " << str << std::endl;

           cl_uint num;
           err = clGetDeviceInfo(device[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &num, nullptr);
           checkErr(err, "Getting device max compute units");
           std::cout << "Max compute units: " << num << std::endl;

           cl_ulong mem_size;
           err = clGetDeviceInfo(device[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &mem_size, nullptr);
           checkErr(err, "Getting device local memory size");
           std::cout << "Local memory size: " << mem_size << std::endl;

           err = clGetDeviceInfo(device[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &mem_size, nullptr);
           checkErr(err, "Getting device global memory size");
           std::cout << "Global memory size: " << mem_size << std::endl;

           err = clGetDeviceInfo(device[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &mem_size, nullptr);
           checkErr(err, "Getting device max allocation size");
           std::cout << "Max allocation size: " << mem_size << std::endl;

           size_t size;
           err = clGetDeviceInfo(device[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &size, nullptr);
           checkErr(err, "Getting device max work-group size");
           std::cout << "Max work-group size: " << size << std::endl;

           err = clGetDeviceInfo(device[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &num, nullptr);
           checkErr(err, "Getting device max work-item dimensions");

           size_t dims[num];
           err = clGetDeviceInfo(device[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(dims), &dims, nullptr);
           checkErr(err, "Getting device max work-item sizes");
           std::cout << "Max work-group dimensions: (" << std::endl;
           for (int k = 0; k < num; k++) {
               std::cout << dims[k] << std::endl;
           }
           std::cout << ")" << std::endl;


       }

    }

    return 0;
}

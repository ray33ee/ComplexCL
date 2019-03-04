#include "complexcanvas.h"

ComplexCanvas::ComplexCanvas(QWidget *parent) : QGraphicsView(parent)
{
    using namespace std;

    cl_uint platCount;
    cl_int res = clGetPlatformIDs(0, nullptr, &platCount);

    cout << "Get platform count. Code " << res << "." << endl;


    if (res == CL_OUT_OF_HOST_MEMORY)
    {

    }

    if (platCount == 0)
    {

    }

    size_t totalCount = 100;

    cl_platform_id platform;
    res = clGetPlatformIDs(1, &platform, nullptr);
    cout << "Get platform. Code " << res << "." << endl;

    cl_device_id device;
    cl_uint devCount;

    res = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &devCount);
    cout << "Get device count. Code " << res << "." << endl;

    res = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, nullptr);
    cout << "Get device. Code " << res << "." << endl;

    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &res);
    cout << "Get context. Code " << res << "." << endl;

    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &res);
    cout << "Get command queue. Code " << res << "." << endl;

    cl_mem buffA = clCreateBuffer(context, 0, totalCount * sizeof(uint32_t), nullptr, &res);
    cout << "Get buffer for A. Code " << res << "." << endl;

    cl_mem buffB = clCreateBuffer(context, 0, totalCount * sizeof(uint32_t), nullptr, &res);
    cout << "Get buffer for B. Code " << res << "." << endl;

    cl_mem resultBuff = clCreateBuffer(context, 0, totalCount * sizeof(uint32_t), nullptr, &res);
    cout << "Get buffer for result. Code " << res << "." << endl;

    uint32_t hostBuffA[totalCount];
    uint32_t hostBuffB[totalCount];

    uint32_t result[totalCount];

    for (uint32_t i = 0; i < totalCount; ++i)
    {
        hostBuffA[i] = i;
        hostBuffB[i] = i * 6;
    }

    res = clEnqueueWriteBuffer(queue, buffA, CL_TRUE, 0, totalCount * sizeof(uint32_t), hostBuffA, 0, nullptr, nullptr);
    cout << "Get copy A data to device. Code " << res << "." << endl;

    res = clEnqueueWriteBuffer(queue, buffB, CL_TRUE, 0, totalCount * sizeof(uint32_t), hostBuffB, 0, nullptr, nullptr);
    cout << "Get copy B data to device. Code " << res << "." << endl;

    const char* kernel = "__kernel void hello_kernel(__global const int *a, __global const int *b, __global int *res)\n"
                        "{ "
                            "int id = get_global_id(0);"
                            "res[id] = a[id] + b[id]; "
                        "}";

    cl_program program = clCreateProgramWithSource(context, 1, &kernel, nullptr, &res);
    cout << "Get program object. Code " << res << "." << endl;

    res = clBuildProgram(program, 1, &device, "", nullptr, nullptr);
    cout << "Build program. Code " << res << "." << endl;

    cl_kernel kern = clCreateKernel(program, "hello_kernel", &res);
    cout << "Create kernel object. Code " << res << "." << endl;

    res = clSetKernelArg(kern, 0, sizeof(uint32_t), &buffA);
    cout << "Add first argument. Code " << res << "." << endl;

    res = clSetKernelArg(kern, 1, sizeof(uint32_t), &buffB);
    cout << "Add first argument. Code " << res << "." << endl;

    res = clSetKernelArg(kern, 2, sizeof(uint32_t), &resultBuff);
    cout << "Add first argument. Code " << res << "." << endl;

    size_t localSize = 1;

    cl_event kernelEvent;

    res = clEnqueueNDRangeKernel(queue, kern, 1, nullptr, &totalCount, &localSize, 0, nullptr, &kernelEvent);
    cout << "Queue kernel. Code " << res << "." << endl;

    res = clWaitForEvents(1, &kernelEvent); //Wait for the kernel to finish
    cout << "Wait... Code " << res << "." << endl;



    res = clEnqueueReadBuffer(queue, resultBuff, CL_TRUE, 0, totalCount * sizeof(uint32_t), result, 0, nullptr, nullptr);
    cout << "Read result. Code " << res << "." << endl;



    for (int i = 0; i < 100; ++i)
    {
        cout << result[i] << endl;
    }



}




























#include "complexcanvas.h"

ComplexCanvas::ComplexCanvas(QWidget *parent) : QGraphicsView(parent), _function(), _maxArea(0)
{
    using namespace std;

    cl_uint platCount;
    cl_platform_id platform;
    cl_device_id device;

    //Put kernel code into const char* pointer
    QFile kernelCode("E:\\Software Projects\\Qt\\ComplexCL\\kernel\\kernel.cl");

    kernelCode.open(QIODevice::ReadOnly);

    QString kernelsource(kernelCode.readAll().data());

    kernelCode.close();

    //Get number of platforms
    _error = clGetPlatformIDs(0, nullptr, &platCount);
    cout << "Get platform count. Code " << _error << "." << endl;

    //If there are no platforms, switch to std::thread usage
    if (platCount == 0)
    {

    }

    getBestDevice(platCount, &device, &platform, true);

    //_error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, nullptr);
    cout << "Get device. Code " << _error << "." << endl;

    _context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &_error);
    cout << "Get context. Code " << _error << "." << endl;

    _queue = clCreateCommandQueue(_context, device, 0, &_error);
    cout << "Get command queue. Code " << _error << "." << endl;

    const char* kernelS = kernelsource.toStdString().data();
    cl_program program = clCreateProgramWithSource(_context, 1, &kernelS, nullptr, &_error);
    cout << "Get program object. Code " << _error << "." << endl;

    _error = clBuildProgram(program, 1, &device, "-I\"E:\\Software Projects\\Qt\\ComplexCL\\inc\"", nullptr, nullptr);
    cout << "Build program. Code " << _error << "." << endl;

    _kernel = clCreateKernel(program, "get_landscape", &_error);
    cout << "Create kernel object. Code " << _error << "." << endl;

    if (_error == CL_INVALID_KERNEL_NAME  )
        cout << "Got ya!" << endl;

    clReleaseProgram(program);

    setScene(&_scene);

    updateFunction(Evaluator(), {-2, -2}, {4, 4});

}

void ComplexCanvas::getBestDevice(int platCount, cl_device_id *device, cl_platform_id *platform, bool fp64)
{
    int maxScore = 0;
    cl_platform_id bestPlatform;
    cl_device_id bestDevice;

    //Iterate over all available devices and determine the most suitable
    cl_platform_id* platforms = new cl_platform_id[platCount];

    cl_int err;

    err = clGetPlatformIDs(platCount, platforms, nullptr);

    for (int pl = 0; pl < platCount; ++pl)
    {
        cl_uint devCount;

        err = clGetDeviceIDs(platforms[pl], CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &devCount);

        cl_device_id* devices = new cl_device_id[devCount];

        err = clGetDeviceIDs(platforms[pl], CL_DEVICE_TYPE_DEFAULT, devCount, devices, nullptr);

        for (int dev = 0; dev < devCount; ++dev)
        {
            char str[1024];
            int score = 1;

            err = clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_CLOCK_FREQUENCY, 1024, str, nullptr);
            score *= *(cl_uint*)str;
            err = clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_WORK_GROUP_SIZE, 1024, str, nullptr);
            score *= *(size_t*)str;
            err = clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_COMPUTE_UNITS, 1024, str, nullptr);
            score *= *(cl_uint*)str;

            if (fp64) //if double precision is required,
            {
                //probe CL_DEVICE_EXTENSIONS
                err = clGetDeviceInfo(devices[dev], CL_DEVICE_EXTENSIONS, 1024, str, nullptr);

                //Test if device allows double precision.If it doesn't, continue
                QString extensions(str);

                if (!extensions.contains("cl_khr_fp64") && !extensions.contains("cl_amd_fp64"))
                    continue;
            }

            //Compare score to current best. if greater, update best platform and device
            if (score > maxScore)
            {
                bestDevice = devices[dev];
                bestPlatform = platforms[pl];

                maxScore = score;
            }

            std::cout << score << std::endl;
        }

        delete[] devices;

    }

    delete[] platforms;

    //If a device with fp64 support was not found, run the search again disabling  fp64 requirment
    if (!maxScore && fp64)
    {
        getBestDevice(platCount, device, platform, false);
        return;
    }

    *device = bestDevice;
    *platform = bestPlatform;
}

int ComplexCanvas::getArea() const
{
    return width() * height();
}

void ComplexCanvas::drawCanvas()
{
    using namespace std;

    size_t w = static_cast<size_t>(width());
    size_t h = static_cast<size_t>(height());
    size_t area = w*h;

    cl_event kernelEvent;

    int stackMax = _function.getStackMax();

    size_t workSize[] = { w, h };

    cl_mem stack = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * stackMax * sizeof(std::complex<double>), nullptr, &_error);
    //cout << "Create stack buffer. Code " << _error << "." << endl;

    _error = 0;

    _error |= clSetKernelArg(_kernel, 1, sizeof(cl_mem), &stack);
    //cout << "Arguments set. Code " << _error << "." << endl;

    _error = clEnqueueNDRangeKernel(_queue, _kernel, 2, nullptr, workSize, nullptr, 1, &_writeEvent, &kernelEvent);
    //cout << "Queue kernel. Code " << _error << "." << endl;

    _error = clEnqueueReadBuffer(_queue, _colourBuff, CL_TRUE, 0, area * sizeof(int), _image.bits(), 0, nullptr, nullptr);
    //cout << "Read result. Code " << _error << "." << endl;

    _scene.clear();
    _scene.addPixmap(QPixmap::fromImage(_image));

    clReleaseMemObject(stack);
}

void ComplexCanvas::updateFunction(Evaluator function, std::complex<double> min, std::complex<double> diff)
{
    using namespace std;

    _function = function;
    _min = min;
    _diff = diff;

    size_t tokenCount = static_cast<size_t>(_function.getCount());

    int stackMax = _function.getStackMax();

    _tokensBuff = clCreateBuffer(_context, CL_MEM_READ_ONLY, tokenCount * sizeof(Token), nullptr, &_error);
    cout << "Create tokens buffer. Code " << _error << "." << endl;

    _error = clEnqueueWriteBuffer(_queue, _tokensBuff, CL_TRUE, 0, tokenCount * sizeof(Token), _function.getTokens(), 0, nullptr, &_writeEvent);
    cout << "Create populate tokens buffer. Code " << _error << "." << endl;


    _error = 0;
    _error |= clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_tokensBuff);
    _error |= clSetKernelArg(_kernel, 2, sizeof(int), &tokenCount);
    _error |= clSetKernelArg(_kernel, 3, sizeof(complex<double>), &_min);
    _error |= clSetKernelArg(_kernel, 4, sizeof(complex<double>), &_diff);
    _error |= clSetKernelArg(_kernel, 9, sizeof(int), &stackMax);

}

void ComplexCanvas::resizeEvent(QResizeEvent *ev)
{
    using namespace std;

    int w = width();
    int h = height();

    int area = w * h;

    _image = QImage(w, h, QImage::Format_RGB32);

    if (area > _maxArea)
    {
        //cout << "rebuff " << endl;
        _colourBuff = clCreateBuffer(_context, CL_MEM_WRITE_ONLY, area * sizeof(int), nullptr, &_error);
        //cout << "Create colour image buffer. Code " << _error << "." << endl;
        _error |= clSetKernelArg(_kernel, 7, sizeof(cl_mem), &_colourBuff);
        _maxArea = area;
    }

    _error = 0;
    _error |= clSetKernelArg(_kernel, 5, sizeof(int), &w);
    _error |= clSetKernelArg(_kernel, 6, sizeof(int), &h);
    _error |= clSetKernelArg(_kernel, 8, sizeof(int), &area);

    drawCanvas();
}

ComplexCanvas::~ComplexCanvas()
{
    clReleaseKernel(_kernel);
    clReleaseEvent(_writeEvent);
    clReleaseContext(_context);
    clReleaseMemObject(_colourBuff);
    clReleaseMemObject(_tokensBuff);
    clReleaseCommandQueue(_queue);
}























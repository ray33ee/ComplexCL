#include "complexcanvas.h"

ComplexCanvas::ComplexCanvas(QWidget *parent) : QGraphicsView(parent), _function(), _maxArea(0)
{
    using namespace std;

    cl_uint platCount;
    cl_platform_id platform;
    cl_device_id device;

    //Put kernel code into const char* pointer
    QFile kernelCode(":/OpenCL/kernel/kernel.cl");

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

    _doublePrecision = getBestDevice(platCount, &device, &platform, true);

    cout << "Double precision? " << (_doublePrecision ? "true" : "false") << endl;

    _context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &_error);
    errHandler(_error, "clCreateContext");
    cout << "Get context." << endl;

    _queue = clCreateCommandQueue(_context, device, 0, &_error);
    errHandler(_error, "clCreateCommandQueue");
    cout << "Get command queue." << endl;


    //If you try to combine the two lines into something like const char* kernelS = kernelsource.toStdString().data()
    //Then it breaks. Don't be smart. Don't do it.
    std::string ks = kernelsource.toStdString();
    const char* kernelS = ks.data();

    //cout << kernelS << endl;

    cl_program program = clCreateProgramWithSource(_context, 1, &kernelS, nullptr, &_error);
    errHandler(_error, "clCreateProgramWithSource");
    cout << "Get program object." << endl;


    _error = clBuildProgram(program, 1, &device, "", nullptr, nullptr);
    errHandler(_error, "clBuildProgram");
    cout << "Build program." << endl;


    _kernel = clCreateKernel(program, "get_landscape", &_error);
    errHandler(_error, "clCreateKernel");
    cout << "Create kernel object." << endl;

    clReleaseProgram(program);

    setScene(&_scene);

updateFunction(Evaluator(), {-10, -10}, {20, 20});

}

void ComplexCanvas::errHandler(cl_int err, const char* string)
{
    if (!err)
        return;

    if (err == CL_OUT_OF_HOST_MEMORY)
    {
        std::cout << "Could not allocate resources required on the host, in " << string << ". Aborting." << std::endl;
        QMessageBox::critical(this, "Out of host memory", "Could not allocate resources required on the host.");
    }
    else
    {
        std::cout << "Error after api call to " << string << ". Error code " << err << "." << std::endl;
        QMessageBox::critical(this, "Critical Error", QString() + "Error after api call to " + string + ". Error code " + QString::number(err) + ".");
    }


    exit(err);
}

bool ComplexCanvas::getBestDevice(int platCount, cl_device_id *device, cl_platform_id *platform, bool fp64)
{
    using namespace std;

    int maxScore = 0;
    cl_platform_id bestPlatform;
    cl_device_id bestDevice;

    //Iterate over all available devices and determine the most suitable
    cl_platform_id* platforms = new cl_platform_id[platCount];

    cl_int err;

    err = clGetPlatformIDs(platCount, platforms, nullptr);
    cout << "Get platform list. Code " << err << endl;

    for (int pl = 0; pl < platCount; ++pl)
    {
        cl_uint devCount;

        err = clGetDeviceIDs(platforms[pl], CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &devCount);
        cout << "Get number of devices for platform " << pl << ". Code " << err << endl;

        cl_device_id* devices = new cl_device_id[devCount];

        err = clGetDeviceIDs(platforms[pl], CL_DEVICE_TYPE_DEFAULT, devCount, devices, nullptr);
        cout << "Get list of devices. Code " << err << endl;

        for (int dev = 0; dev < devCount; ++dev)
        {
            char str[1024];
            int score = 1;

            err = 0;

            err |= clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_CLOCK_FREQUENCY, 1024, str, nullptr);
            score *= *reinterpret_cast<cl_uint*>(str);
            err |= clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_WORK_GROUP_SIZE, 1024, str, nullptr);
            score *= *reinterpret_cast<size_t*>(str);
            err |= clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_COMPUTE_UNITS, 1024, str, nullptr);
            score *= *reinterpret_cast<cl_uint*>(str);

            cout << "    Get device " << dev << " score (" << score << "). Code " << err << endl;


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
        }

        delete[] devices;

    }

    delete[] platforms;

    //If a device with fp64 support was not found, run the search again disabling  fp64 requirment
    if (!maxScore && fp64)
    {
        return getBestDevice(platCount, device, platform, false);
    }

    *device = bestDevice;
    *platform = bestPlatform;

    return fp64;
}

int ComplexCanvas::getArea() const
{
    return width() * height();
}

void ComplexCanvas::drawCanvas()
{
    using namespace std;

    size_t w = static_cast<size_t>(width());
    size_t h = static_cast<size_t>(height()-36);
    size_t area = w*h;

    cl_event kernelEvent;

    int stackMax = _function.getStackMax();

    size_t workSize[] = { w, h };

    cl_mem stack;

    if (_doublePrecision)
        stack = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * stackMax * sizeof(std::complex<double>), nullptr, &_error);
    else
        stack = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * stackMax * sizeof(std::complex<float>), nullptr, &_error);

    //cout << "Create stack buffer. Code " << _error << "." << endl;

    _error = 0;

    _error |= clSetKernelArg(_kernel, 1, sizeof(cl_mem), &stack);
    //cout << "Arguments set. Code " << _error << "." << endl;

    _error = clEnqueueNDRangeKernel(_queue, _kernel, 2, nullptr, workSize, nullptr, 1, &_writeEvent, &kernelEvent);
    //cout << "Queue kernel. Code " << _error << "." << endl;

    _error = clEnqueueReadBuffer(_queue, _colourBuff, CL_TRUE, 0, area * sizeof(int), _image.bits(), 1, &kernelEvent, nullptr);
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

    _tokensBuff = clCreateBuffer(_context, CL_MEM_READ_ONLY, tokenCount * sizeof(Token<double>), nullptr, &_error);
    cout << "Create tokens buffer. Code " << _error << "." << endl;

    if (_doublePrecision)
        _error = clEnqueueWriteBuffer(_queue, _tokensBuff, CL_TRUE, 0, tokenCount * sizeof(Token<double>), _function.getTokens(), 0, nullptr, &_writeEvent);
    else
        _error = clEnqueueWriteBuffer(_queue, _tokensBuff, CL_TRUE, 0, tokenCount * sizeof(Token<float>), _function.getFloatTokens(), 0, nullptr, &_writeEvent);

    cout << "Create populate tokens buffer. Code " << _error << "." << endl;


    _error = 0;
    _error |= clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_tokensBuff);
    _error |= clSetKernelArg(_kernel, 2, sizeof(int), &tokenCount);

    if (_doublePrecision)
    {
        _error |= clSetKernelArg(_kernel, 3, sizeof(complex<double>), &_min);
        _error |= clSetKernelArg(_kernel, 4, sizeof(complex<double>), &_diff);
    }
    else
    {
        complex<float> minf(static_cast<float>(_min.real()), static_cast<float>(_min.imag()));
        complex<float> difff(static_cast<float>(_diff.real()), static_cast<float>(_diff.imag()));

        _error |= clSetKernelArg(_kernel, 3, sizeof(complex<float>), &minf);
        _error |= clSetKernelArg(_kernel, 4, sizeof(complex<float>), &difff);
    }
    _error |= clSetKernelArg(_kernel, 9, sizeof(int), &stackMax);


    errHandler(_error, "clSetKernelArg");

}

void ComplexCanvas::resizeEvent(QResizeEvent *)
{
    using namespace std;

    int w = width();
    int h = height()-36;

    int area = w * h;

    _image = QImage(w, h, QImage::Format_RGB32);

    _error = 0;

    if (area > _maxArea)
    {
        _colourBuff = clCreateBuffer(_context, CL_MEM_WRITE_ONLY, area * sizeof(int), nullptr, &_error);
        errHandler(_error, "clCreateBuffer");
        _error = 0;
        //cout << "Create colour image buffer. Code " << _error << "." << endl;
        _error |= clSetKernelArg(_kernel, 7, sizeof(cl_mem), &_colourBuff);
        _maxArea = area;
    }

    _error = 0;
    _error |= clSetKernelArg(_kernel, 5, sizeof(int), &w);
    _error |= clSetKernelArg(_kernel, 6, sizeof(int), &h);
    _error |= clSetKernelArg(_kernel, 8, sizeof(int), &area);

    errHandler(_error, "clSetKernelArg");

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























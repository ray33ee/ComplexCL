#include "complexcanvas.h"

ComplexCanvas::ComplexCanvas(QWidget *parent) : QGraphicsView(parent), _function()
{
    using namespace std;

    QFile kernelCode("E:\\Software Projects\\Qt\\ComplexCL\\kernel\\kernel.cl");

    kernelCode.open(QIODevice::ReadOnly);

    QString kernelsource(kernelCode.readAll().data());

    kernelCode.close();

    _image = new QImage(width() - 2, height() - 2, QImage::Format_RGB32);

    cl_uint platCount;
    cl_int err = clGetPlatformIDs(0, nullptr, &platCount);

    cout << "Get platform count. Code " << _error << "." << endl;


    if (_error == CL_INVALID_VALUE )
    {
        cout << "pokpokpo";
    }

    if (platCount == 0)
    {

    }

    cl_platform_id platform;
    _error = clGetPlatformIDs(1, &platform, nullptr);
    cout << "Get platform. Code " << _error << "." << endl;

    cl_uint devCount;

    _error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &devCount);
    cout << "Get device count. Code " << _error << "." << endl;

    _error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &_device, nullptr);
    cout << "Get device. Code " << _error << "." << endl;

    _context = clCreateContext(nullptr, 1, &_device, nullptr, nullptr, &_error);
    cout << "Get context. Code " << _error << "." << endl;

    _queue = clCreateCommandQueue(_context, _device, 0, &_error);
    cout << "Get command queue. Code " << _error << "." << endl;

    const char* kernelS = kernelsource.toStdString().data();
    _program = clCreateProgramWithSource(_context, 1, &kernelS, nullptr, &_error);
    cout << "Get program object. Code " << _error << "." << endl;

    _error = clBuildProgram(_program, 1, &_device, "-I\"E:\\Software Projects\\Qt\\ComplexCL\\inc\"", nullptr, nullptr);
    cout << "Build program. Code " << _error << "." << endl;

    _kernel = clCreateKernel(_program, "get_landscape", &_error);
    cout << "Create kernel object. Code " << _error << "." << endl;

    if (_error == CL_INVALID_KERNEL_NAME  )
        cout << "Got ya!" << endl;




    drawCanvas();
}

int ComplexCanvas::getArea() const
{
    return width() * height();
}

void ComplexCanvas::drawCanvas()
{
    using namespace std;

    int tokenCount = _function.getCount();

    int w = 381 - 5;
    int h = 221 - 5;
    int area = w*h;

    cl_event writeEvent, kernelEvent;

    cl_mem tokens = clCreateBuffer(_context, CL_MEM_READ_WRITE, tokenCount * sizeof(Token), nullptr, &_error);
    cout << "Create tokens buffer. Code " << _error << "." << endl;

    cl_mem stack = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * _function.getStackMax() * sizeof(std::complex<double>), nullptr, &_error);
    cout << "Create stack buffer. Code " << _error << "." << endl;

    cl_mem colours = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * sizeof(int), nullptr, &_error);
    cout << "Create colour image buffer. Code " << _error << "." << endl;

    _error = clEnqueueWriteBuffer(_queue, tokens, CL_TRUE, 0, tokenCount * sizeof(Token), _function.getTokens(), 0, nullptr, &writeEvent);
    cout << "Create populate tokens buffer. Code " << _error << "." << endl;


    double minRe = -100.0;
    double minIm = -100.0;
    double diffRe = 200.0;
    double diffIm = 200.0;

    int stackMax = _function.getStackMax();

    size_t localSize = 1;
    size_t totalSize = area;

    size_t workSize[] = { (size_t)w, (size_t)h };

    _error = 0;

    _error |= clSetKernelArg(_kernel, 0, sizeof(cl_mem), &tokens);
    _error |= clSetKernelArg(_kernel, 1, sizeof(cl_mem), &stack);
    _error |= clSetKernelArg(_kernel, 2, sizeof(int), &tokenCount);
    _error |= clSetKernelArg(_kernel, 3, sizeof(double), &minRe);
    _error |= clSetKernelArg(_kernel, 4, sizeof(double), &minIm);
    _error |= clSetKernelArg(_kernel, 5, sizeof(double), &diffRe);
    _error |= clSetKernelArg(_kernel, 6, sizeof(double), &diffIm);
    _error |= clSetKernelArg(_kernel, 7, sizeof(int), &w);
    _error |= clSetKernelArg(_kernel, 8, sizeof(int), &h);
    _error |= clSetKernelArg(_kernel, 9, sizeof(cl_mem), &colours);
    _error |= clSetKernelArg(_kernel, 10, sizeof(int), &area);
    _error |= clSetKernelArg(_kernel, 11, sizeof(int), &stackMax);

    cout << "Arguments set. Code " << _error << "." << endl;

    int* cols = new int[area];

    cout << "Area: " << width() << " " << height() << endl;

    _error = clEnqueueNDRangeKernel(_queue, _kernel, 2, nullptr, workSize, &localSize, 1, &writeEvent, &kernelEvent);
    cout << "Queue kernel. Code " << _error << "." << endl;

    _error = clEnqueueReadBuffer(_queue, colours, CL_TRUE, 0, area * sizeof(int), cols, 0, nullptr, nullptr);
    cout << "Read result. Code " << _error << "." << endl;



    _image = new QImage((uchar*)cols, w, h, QImage::Format_RGB32);

    cout << cols[325] << endl;
    cout << _function.getTokens()[0]._type << endl;

    _scene.addPixmap(QPixmap::fromImage(*_image));

    setScene(&_scene);

}


























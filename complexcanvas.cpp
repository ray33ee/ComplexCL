#include "complexcanvas.h"

ComplexCanvas::ComplexCanvas(QWidget *parent) : QGraphicsView(parent), _function(), _maxArea(0)
{
    using namespace std;

    QFile kernelCode("E:\\Software Projects\\Qt\\ComplexCL\\kernel\\kernel.cl");

    kernelCode.open(QIODevice::ReadOnly);

    QString kernelsource(kernelCode.readAll().data());

    kernelCode.close();

    cl_uint platCount;
    _error = clGetPlatformIDs(0, nullptr, &platCount);
    cout << "Get platform count. Code " << _error << "." << endl;

    if (platCount == 0)
    {

    }

    cl_platform_id platform;
    _error = clGetPlatformIDs(1, &platform, nullptr);
    cout << "Get platform. Code " << _error << "." << endl;

    cl_uint devCount;

    _error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &devCount);
    cout << "Get device count. Code " << _error << "." << endl;

    cl_device_id device;

    _error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, nullptr);
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


    setScene(&_scene);

    updateFunction(Evaluator(), {-2, -2}, {4, 4});

}

int ComplexCanvas::getArea() const
{
    return width() * height();
}

void ComplexCanvas::drawCanvas()
{
    using namespace std;

    size_t w = width();
    size_t h = height();
    size_t area = w*h;

    size_t tokenCount = static_cast<size_t>(_function.getCount());

    cl_event kernelEvent;

    int stackMax = _function.getStackMax();

    size_t localSize = 1;

    size_t workSize[] = { w, h };

    cl_mem stack = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * stackMax * sizeof(std::complex<double>), nullptr, &_error);
    //cout << "Create stack buffer. Code " << _error << "." << endl;

    _error = 0;

    _error |= clSetKernelArg(_kernel, 1, sizeof(cl_mem), &stack);
    _error |= clSetKernelArg(_kernel, 9, sizeof(int), &stackMax);

    //cout << "Arguments set. Code " << _error << "." << endl;

    _error = clEnqueueNDRangeKernel(_queue, _kernel, 2, nullptr, workSize, &localSize, 1, &_writeEvent, &kernelEvent);
    //cout << "Queue kernel. Code " << _error << "." << endl;

    _error = clEnqueueReadBuffer(_queue, _colourBuff, CL_TRUE, 0, area * sizeof(int), _image.bits(), 0, nullptr, nullptr);
    //cout << "Read result. Code " << _error << "." << endl;

    _scene.addPixmap(QPixmap::fromImage(_image));


}

void ComplexCanvas::updateFunction(Evaluator function, std::complex<double> min, std::complex<double> diff)
{
    using namespace std;

    _function = function;
    _min = min;
    _diff = diff;

    size_t tokenCount = static_cast<size_t>(_function.getCount());

    int stackMax = _function.getStackMax();

    _tokensBuff = clCreateBuffer(_context, CL_MEM_READ_WRITE, tokenCount * sizeof(Token), nullptr, &_error);
    cout << "Create tokens buffer. Code " << _error << "." << endl;

    _error = clEnqueueWriteBuffer(_queue, _tokensBuff, CL_TRUE, 0, tokenCount * sizeof(Token), _function.getTokens(), 0, nullptr, &_writeEvent);
    cout << "Create populate tokens buffer. Code " << _error << "." << endl;


    _error |= clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_tokensBuff);
    _error |= clSetKernelArg(_kernel, 2, sizeof(int), &tokenCount);
    _error |= clSetKernelArg(_kernel, 3, sizeof(complex<double>), &_min);
    _error |= clSetKernelArg(_kernel, 4, sizeof(complex<double>), &_diff);

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
        cout << "rebuff " << endl;
        _colourBuff = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * sizeof(int), nullptr, &_error);
        _error |= clSetKernelArg(_kernel, 7, sizeof(cl_mem), &_colourBuff);
        _maxArea = area;
    }
    //cout << "Create colour image buffer. Code " << _error << "." << endl;

    _error |= clSetKernelArg(_kernel, 5, sizeof(int), &w);
    _error |= clSetKernelArg(_kernel, 6, sizeof(int), &h);
    _error |= clSetKernelArg(_kernel, 8, sizeof(int), &area);

    drawCanvas();
}

























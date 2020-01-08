#include "complexcanvas.h"
#include "mainwindow.h"


ComplexCanvas::ComplexCanvas(QWidget *parent) : QGraphicsView(parent), _land(Landscape()), _maxArea(0)
{
    setup(true);

    setMouseTracking(true);

    setScene(&_scene);

    drawLandscape();

}

void ComplexCanvas::setup(bool fp64)
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
    logAppend(QString("Get platform count. Code " + QString::number(_error) + "."));
    //cout << "Get platform count. Code " << _error << "." << endl;

    //If there are no platforms, switch to std::thread usage
    if (platCount == 0)
    {

    }

    _doublePrecision = getBestDevice(platCount, &device, &platform, fp64);

    logAppend(QString("Double precision? ") + (_doublePrecision ? "true" : "false"));

    _context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &_error);
    errHandler(_error, "clCreateContext");
    logAppend(QString("Get context."));

    _queue = clCreateCommandQueue(_context, device, 0, &_error);
    errHandler(_error, "clCreateCommandQueue");
    logAppend(QString("Get command queue."));


    //If you try to combine the two lines into something like const char* kernelS = kernelsource.toStdString().data()
    //Then it breaks. Don't be smart. Don't do it.
    std::string ks = kernelsource.toStdString();
    const char* kernelS = ks.data();

    //cout << kernelS << endl;


    cl_program program = clCreateProgramWithSource(_context, 1, &kernelS, nullptr, &_error);
    errHandler(_error, "clCreateProgramWithSource");
    logAppend(QString("Get program object."));


    _error = clBuildProgram(program, 1, &device, "", nullptr, nullptr);
    errHandler(_error, "clBuildProgram");
    logAppend(QString("Build program."));


    _kernel = clCreateKernel(program, "get_landscape", &_error);
    errHandler(_error, "clCreateKernel");
    logAppend(QString("Create kernel object."));

    clReleaseProgram(program);

    drawLandscape();
}

void ComplexCanvas::logAppend(QString line)
{
    QFile file("log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&file);

    //Uncomment this next line for writing log entries to qDebug output stream
    qDebug() << QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss ") << line;
    //Uncomment this next line for writing log entries to log.txt
    //out << QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm ") << line << "\n";

    file.close();

}

void ComplexCanvas::errHandler(cl_int err, const char* string)
{
    using namespace std;

    if (!err)
        return;

    if (err == CL_OUT_OF_HOST_MEMORY)
    {
        cout << "Could not allocate resources required on the host, in " << string << ". Aborting." << endl;
        QMessageBox::critical(this, "Out of host memory", "Could not allocate resources required on the host.");
    }
    else
    {
        cout << "Error after api call to " << string << ". Error code " << err << "." << endl;
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
    logAppend(QString("Get platform list. Code ") + QString::number(err));

    for (int pl = 0; pl < platCount; ++pl)
    {
        cl_uint devCount;

        err = clGetDeviceIDs(platforms[pl], CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &devCount);
        logAppend(QString("Get number of devices for platform ") + QString::number(pl) + ". Code " + QString::number(err));

        cl_device_id* devices = new cl_device_id[devCount];

        err = clGetDeviceIDs(platforms[pl], CL_DEVICE_TYPE_DEFAULT, devCount, devices, nullptr);
        logAppend(QString("Get list of devices. Code ") + QString::number(err));

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

            err |= clGetDeviceInfo(devices[dev], CL_DEVICE_NAME, 1024, str, nullptr);

            logAppend(QString("    Get device ") + str + " score (" + QString::number(score) + "). Code " + QString::number(err));


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

int ComplexCanvas::width() const
{
    return QGraphicsView::width() - 2;
}

int ComplexCanvas::height() const
{
    return QGraphicsView::height() - 2;
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

    int stackMax = _land.getStackMax();

    cl_mem stack;

    if (_doublePrecision)
        stack = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * stackMax * sizeof(std::complex<double>), nullptr, &_error);
    else
        stack = clCreateBuffer(_context, CL_MEM_READ_WRITE, area * stackMax * sizeof(std::complex<float>), nullptr, &_error);

    //logAppend(QString("Create stack buffer. Code ") + QString::number(_error) + ".");

    _error = 0;

    _error |= clSetKernelArg(_kernel, 1, sizeof(cl_mem), &stack);
    //logAppend(QString("Arguments set. Code ") + QString::number(_error) + ".");



    _error = clEnqueueNDRangeKernel(_queue, _kernel, 1, nullptr, &area, nullptr, 1, &_writeEvent, &kernelEvent);
    //logAppend(QString("Queue kernel. Code ") + QString::number(_error) + ".");

    if (_error == CL_INVALID_KERNEL_ARGS)
        cout << "wtf" << endl;

    QTime tmr;
    tmr.start();

    _error = clWaitForEvents(1, &kernelEvent);
    logAppend(QString("Waiting for kernel. Code ") + QString::number(_error) + ".");

#ifdef QT_DEBUG
    logAppend(QString("Elapsed: ") + QString::number(tmr.elapsed()));
#endif

    _error = clEnqueueReadBuffer(_queue, _colourBuff, CL_TRUE, 0, area * sizeof(int), _image.bits(), 1, &kernelEvent, nullptr);
    logAppend(QString("Read result. Code ") + QString::number(_error) + ".");



    _scene.clear();
    _scene.addPixmap(QPixmap::fromImage(_image));



    clReleaseMemObject(stack);
}

void ComplexCanvas::drawLandscape()
{
    using namespace std;

    size_t tokenCount = static_cast<size_t>(_land.getCount());

    int stackMax = _land.getStackMax();

    _tokensBuff = clCreateBuffer(_context, CL_MEM_READ_ONLY, tokenCount * sizeof(Token<double>), nullptr, &_error);
    logAppend(QString("Create tokens buffer. Code ") + QString::number(_error) + ".");

    if (_doublePrecision)
        _error = clEnqueueWriteBuffer(_queue, _tokensBuff, CL_TRUE, 0, tokenCount * sizeof(Token<double>), _land.getTokens(), 0, nullptr, &_writeEvent);
    else
        _error = clEnqueueWriteBuffer(_queue, _tokensBuff, CL_TRUE, 0, tokenCount * sizeof(Token<float>), _land.getFloatTokens(), 0, nullptr, &_writeEvent);

    logAppend(QString("Create populate tokens buffer. Code ") + QString::number(_error));


    _error = 0;
    _error |= clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_tokensBuff);
    _error |= clSetKernelArg(_kernel, 2, sizeof(int), &tokenCount);

    auto min = (_land.getMin());
    auto diff = _land.getDiff();

    if (_doublePrecision)
    {
        _error |= clSetKernelArg(_kernel, 3, sizeof(complex<double>), &min);
        _error |= clSetKernelArg(_kernel, 4, sizeof(complex<double>), &diff);
    }
    else
    {
        complex<float> minf = static_cast<complex<float> >(min);
        complex<float> difff = static_cast<complex<float> >(diff);

        _error |= clSetKernelArg(_kernel, 3, sizeof(complex<float>), &minf);
        _error |= clSetKernelArg(_kernel, 4, sizeof(complex<float>), &difff);
    }
    _error |= clSetKernelArg(_kernel, 9, sizeof(int), &stackMax);

    logAppend(QString("Set kernel arguments (error is or'd with multiple calls) ") + QString::number(_error));

    errHandler(_error, "clSetKernelArg");

    drawCanvas();

}

void ComplexCanvas::drawLandscape(Landscape land)
{
    _land = land;

    MainWindow* win = dynamic_cast<MainWindow*>(parent()->parent());

    win->setLandscape(_land);

    drawLandscape();
}

void ComplexCanvas::resizeEvent(QResizeEvent *)
{
    using namespace std;

    int w = width();
    int h = height();

    int area = w * h;

    _image = QImage(w, h, QImage::Format_RGB32);

    _error = 0;

    if (area > _maxArea)
    {
        _colourBuff = clCreateBuffer(_context, CL_MEM_WRITE_ONLY, area * sizeof(int), nullptr, &_error);
        logAppend(QString("Create output image buffer ") + QString::number(_error));
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

    logAppend(QString("Set kernel arguments (error is or'd with multiple calls) ") + QString::number(_error));

    errHandler(_error, "clSetKernelArg");

    drawCanvas();
}

std::complex<double> ComplexCanvas::interpolate(QMouseEvent* mouse)
{

    double tx = mouse->x();
    double ty = height() - mouse->y();

    //qDebug() << "    Size: " << _land.getMin().real() << " " << _land.getMin().imag() << " " << _land.getDiff().real() << " " << _land.getDiff().imag();

    std::complex<double> ans = std::complex<double>(_land.getMin().real() + tx * _land.getDiff().real() / width(),
                                                    _land.getMin().imag() + ty * _land.getDiff().imag() / height());

    return ans;
}

void ComplexCanvas::mousePressEvent(QMouseEvent * ev)
{
     _pressVector = interpolate(ev);

     //qDebug() << "    Press: " << _pressVector.real() << " " << _pressVector.imag();
}

void ComplexCanvas::mouseReleaseEvent(QMouseEvent * ev)
{
    auto thisVector = interpolate(ev);
    auto diffVector = thisVector - _pressVector;

    MainWindow* win = (MainWindow*)parent()->parent();


    if (_mode == PAN)
    {
        _land.setMinDiff(_land.getMin() - diffVector, _land.getDiff());

        win->add(_land);
    }
    else if (_mode == ZOOM)
    {
        auto min = std::complex<double>(std::min(_pressVector.real(), thisVector.real()), std::min(_pressVector.imag(), thisVector.imag()));
        auto max = std::complex<double>(std::max(_pressVector.real(), thisVector.real()), std::max(_pressVector.imag(), thisVector.imag()));

        _land.setDomain(min, max);
        win->add(_land);

    }
    else //_mode == NEWTON
    {

    }

    win->setLandscape(_land);

    drawLandscape();

}

void ComplexCanvas::mouseMoveEvent(QMouseEvent *event)
{
    auto move = interpolate(event);

    MainWindow* win = (MainWindow*)parent()->parent();

    win->trace(move);
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























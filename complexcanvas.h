#ifndef COMPLEXCANVAS_H
#define COMPLEXCANVAS_H

#include <QGraphicsView>
#include <CL/cl.h>
#include <iostream>
#include <QFile>
#include "landscape.h"
#include <complex>
#include <QDebug>
#include <QResizeEvent>
#include <QMessageBox>
#include <QCoreApplication>

/**
 * @brief The ComplexCanvas class is responsible for drawing the complex landscape.
 * Prefering OpenCL it can use multiparalell processing to quickly generate the pixmap needed.
 * If no OpenCL platforms are present, the canvas instead uses native c++ multithreading code
 */
class ComplexCanvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ComplexCanvas(QWidget *parent = nullptr);

    int getArea() const;
    int width() const;
    int height() const;

    void drawCanvas();

    void updateFunction(Landscape land);

    virtual void resizeEvent(QResizeEvent *ev);

    ~ComplexCanvas();

private:

    //OpenCL context, command queue and kernel
    cl_context          _context;
    cl_command_queue    _queue;
    cl_kernel           _kernel;

    //OpenCL Buffers
    cl_mem              _colourBuff;
    cl_mem              _tokensBuff;

    //OpenCL events
    cl_event            _writeEvent;

    //OpenCL error code
    cl_int              _error;

    //Graphics
    QGraphicsScene      _scene;
    QImage              _image;

    //Landscape to plot
    Landscape           _land;

    int                 _maxArea;

    bool                _doublePrecision;

    static bool getBestDevice(int platCount, cl_device_id *device, cl_platform_id *platform, bool fp64);

    void errHandler(cl_int err, const char* string);


signals:

public slots:
};

#endif // COMPLEXCANVAS_H

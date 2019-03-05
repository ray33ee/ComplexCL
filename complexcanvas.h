#ifndef COMPLEXCANVAS_H
#define COMPLEXCANVAS_H

#include <QGraphicsView>
#include <CL/cl.h>
#include <iostream>
#include <QFile>
#include "evaluator.h"

#include <QDebug>

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

    void drawCanvas();

    ~ComplexCanvas() {}

private:
    cl_device_id        _device;
    cl_context          _context;
    cl_command_queue    _queue;
    cl_program          _program;
    cl_int              _error;
    cl_kernel           _kernel;
    QImage*              _image;
    QGraphicsScene      _scene;

    Evaluator           _function;

signals:

public slots:
};

#endif // COMPLEXCANVAS_H

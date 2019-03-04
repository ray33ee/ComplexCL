#ifndef COMPLEXCANVAS_H
#define COMPLEXCANVAS_H

#include <QGraphicsView>
#include <CL/cl.h>
#include <iostream>

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

signals:

public slots:
};

#endif // COMPLEXCANVAS_H

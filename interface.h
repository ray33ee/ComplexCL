#ifndef INTERFACE_H
#define INTERFACE_H

#include <complex>
#include "landscape.h"

enum Mode
{
    PAN, ZOOM, NEWTON
};

/**
 * @brief The interface class exposes the functions used to communicate between the widgets
 */
class Interface
{
public:
    Interface() {}

    virtual void dialog() = 0;

    virtual void trace(const std::complex<double> &z) = 0;

    virtual void setMode(Mode mode) = 0;
    virtual void zoom(double factor) = 0;

    virtual void centre() = 0;

    virtual void setEnabled(bool b) = 0;

    virtual void undo() = 0;

    virtual void redo() = 0;

    virtual void add(const Landscape &) = 0;

    virtual void history() = 0;

    //virtual void revert();

    //~Interface() = 0;
};

#endif // INTERFACE_H

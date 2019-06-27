#include "customstatusbar.h"

CustomStatusbar::CustomStatusbar(QWidget* parent): QStatusBar (parent)
{
    traceReal = new QLabel("real", this);
    traceImag = new QLabel("imaginary", this);
    //traceColour = new QLabel("   ");

    addPermanentWidget(traceReal);
    addPermanentWidget(traceImag);
}

void CustomStatusbar::trace(const std::complex<double> &z)
{
    traceReal->setText(QString::number(z.real()));
    traceImag->setText(QString::number(z.imag()));
}

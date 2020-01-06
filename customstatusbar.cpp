#include "customstatusbar.h"

CustomStatusbar::CustomStatusbar(QWidget* parent): QStatusBar (parent)
{
    traceReal = new QLabel("real", this);
    traceImag = new QLabel("imaginary", this);
    //traceColour = new QLabel("   ");

    traceReal->setScaledContents(false);
    traceImag->setScaledContents(false);

    traceReal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    traceImag->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    traceReal->resize(100, 40);
    traceImag->resize(100, 40);

    addPermanentWidget(traceReal);
    addPermanentWidget(traceImag);


}

void CustomStatusbar::trace(const std::complex<double> &z)
{
    traceReal->setText(QString::number(z.real()));
    traceImag->setText(QString::number(z.imag()));
}

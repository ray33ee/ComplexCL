#include "customstatusbar.h"

CustomStatusbar::CustomStatusbar(QWidget* parent): QStatusBar (parent)
{

    function = new QLabel("f(z) = z", this);

    traceInput = new QLabel("input", this);
    traceOutput = new QLabel("output", this);
    //traceColour = new QLabel("   ");

    polarOutput = new QLabel("polar", this);

    for (auto &label : findChildren<QLabel*>())
    {
        label->setScaledContents(false);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        label->resize(200, 40);
        addPermanentWidget(label);

    }

}

void CustomStatusbar::setLandscape(Landscape* land)
{
    function->setText(land->toString().remove(QRegExp("[\n]")));
}

void CustomStatusbar::trace(const std::complex<double> &z, const std::complex<double> &w)
{
    traceInput->setText(std::toString(z, 5));
    traceOutput->setText(std::toString(w, 5));
    polarOutput->setText(std::toPolarString(w));
}

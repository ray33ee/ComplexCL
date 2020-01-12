#ifndef CUSTOMSTATUSBAR_H
#define CUSTOMSTATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <landscape.h>

class CustomStatusbar : public QStatusBar
{
private:

    QLabel* traceInput;
    QLabel* traceOutput;

    QLabel* traceColour;

    QLabel* polarOutput;

    QLabel* function;


public:
    CustomStatusbar(QWidget* parent = nullptr);

    void setLandscape(Landscape* land);

    void trace(const std::complex<double> &z, const std::complex<double> &w);
};

#endif // CUSTOMSTATUSBAR_H

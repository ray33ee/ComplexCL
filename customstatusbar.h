#ifndef CUSTOMSTATUSBAR_H
#define CUSTOMSTATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <landscape.h>

class CustomStatusbar : public QStatusBar
{
private:
    Landscape* landscape; //Pointer to current landscape in use

    QLabel* traceReal;
    QLabel* traceImag;

public:
    CustomStatusbar(QWidget* parent = nullptr);

    void setLandscape(Landscape* land) { landscape = land; }

    void trace(const std::complex<double> &z);
};

#endif // CUSTOMSTATUSBAR_H

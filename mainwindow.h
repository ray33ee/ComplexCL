#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <newdialog.h>
#include <complexcanvas.h>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void resizeEvent(QResizeEvent *e);

    //Ui::MainWindow *getUi() { return ui; }

    void newButtonClick();

    void trace(const std::complex<double> &z);

    void setMode(ComplexCanvas::Mode mode);

    void zoom(double factor);

    void centre();

    void setLandscape(const Landscape &land) { _newdialog->setLandscape(land); }

private:
    Ui::MainWindow *ui;

    NewDialog* _newdialog;
};

#endif // MAINWINDOW_H

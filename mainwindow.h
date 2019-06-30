#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <newdialog.h>
#include <complexcanvas.h>
#include "interface.h"
#include "landscape.h"
#include "history.h"
#include "historydialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Interface
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual void resizeEvent(QResizeEvent *e);

    void setLandscape(const Landscape &land) { _newdialog->setLandscape(land); }



    void dialog();

    void trace(const std::complex<double> &z);

    void setMode(Mode mode);

    void zoom(double factor);

    void centre();

    void setEnabled(bool b);


    void add(const Landscape &land);

    void redo();

    void undo();

    void history();

private:
    Ui::MainWindow *ui;

    History<Landscape> _history;

    NewDialog* _newdialog;
    HistoryDialog* _historydialog;
};

#endif // MAINWINDOW_H

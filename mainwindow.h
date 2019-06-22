#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <newdialog.h>

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

private:
    Ui::MainWindow *ui;

    NewDialog* _newdialog;
};

#endif // MAINWINDOW_H

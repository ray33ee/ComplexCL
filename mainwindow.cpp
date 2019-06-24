#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _newdialog = new NewDialog(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->complexcanvas->setGeometry(ui->complexcanvas->geometry().x(), ui->complexcanvas->geometry().y(), width() - 15, height() - 110);
}

void MainWindow::newButtonClick()
{
    _newdialog->exec();

    qDebug() << _newdialog->getLandscape().toString();

    ui->complexcanvas->updateFunction(_newdialog->getLandscape());

    ui->complexcanvas->drawCanvas();
}

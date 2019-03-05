#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    ui->complexcanvas->setGeometry(ui->complexcanvas->geometry().x(), ui->complexcanvas->geometry().y(), width() - 20, height() - 5);
}

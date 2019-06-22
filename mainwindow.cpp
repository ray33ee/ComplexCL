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
    ui->complexcanvas->setGeometry(ui->complexcanvas->geometry().x(), ui->complexcanvas->geometry().y(), width() - 20, height() - 5);
}

void MainWindow::newButtonClick()
{
    _newdialog->exec();

    qDebug() << "Min: " << _newdialog->getMin().real() << " " << _newdialog->getMin().imag();
    qDebug() << "Max: " << _newdialog->getMax().real() << " " << _newdialog->getMax().imag();
    qDebug() << "Func: " << _newdialog->getEvaluator().getFormula();

    ui->complexcanvas->updateFunction(_newdialog->getEvaluator(), _newdialog->getMin(), _newdialog->getMax() - _newdialog->getMin());

    ui->complexcanvas->drawCanvas();
}

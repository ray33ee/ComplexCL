#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _newdialog = new NewDialog(this);

    ui->statusBar->setLandscape(ui->complexcanvas->getReference());

    setObjectName("Main WIndow");
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
    _newdialog->setLandscape(ui->complexcanvas->getLandscape());

    _newdialog->exec();

    qDebug() << _newdialog->getLandscape().toString();

    ui->complexcanvas->drawLandscape(_newdialog->getLandscape());
}

void MainWindow::setMode(ComplexCanvas::Mode mode)
{
    ui->complexcanvas->_mode = mode;
}

void MainWindow::trace(const std::complex<double> &z)
{
    ui->statusBar->trace(z);
}

void MainWindow::zoom(double factor)
{
    Landscape land = ui->complexcanvas->getLandscape();

    std::complex<double> ndiff = land.getDiff() * factor;
    std::complex<double> nmin = std::complex<double>(land.getMin().real() + land.getDiff().real() * (1.0-factor) / 2.0,
                                                     land.getMin().imag() + land.getDiff().imag() * (1.0-factor) / 2.0);

    land.setMinDiff(nmin, ndiff);

    ui->complexcanvas->drawLandscape(land);

    qDebug() << toString(nmin) << " " << toString(ndiff) ;
}

void MainWindow::centre()
{

}

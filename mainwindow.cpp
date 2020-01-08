#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _newdialog = new NewDialog(this);

    _historydialog = new HistoryDialog(this);

    _calculatordialog = new CalculatorDialog(this);

    ui->statusBar->setLandscape(ui->complexcanvas->getReference());

    add(Landscape());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    //auto i = ui->complexcanvas;
    ui->complexcanvas->setGeometry(ui->complexcanvas->geometry().x(), ui->complexcanvas->geometry().y(), width() - 15, height() - 110);
}

void MainWindow::dialog()
{
    _newdialog->setLandscape(ui->complexcanvas->getLandscape());

    _newdialog->exec();

    ui->complexcanvas->drawLandscape(_newdialog->getLandscape());

    ui->statusBar->setLandscape(ui->complexcanvas->getReference());

    add(_newdialog->getLandscape());
}

void MainWindow::setMode(Mode mode)
{
    ui->complexcanvas->_mode = mode;
}

void MainWindow::trace(const std::complex<double> &z)
{
    std::complex<double> w = (ui->complexcanvas->getLandscape())(z);
    ui->statusBar->trace(z, w);
}

void MainWindow::zoom(double factor)
{
    Landscape land = ui->complexcanvas->getLandscape();

    std::complex<double> ndiff = land.getDiff() * factor;
    std::complex<double> nmin = std::complex<double>(land.getMin().real() + land.getDiff().real() * (1.0-factor) / 2.0,
                                                     land.getMin().imag() + land.getDiff().imag() * (1.0-factor) / 2.0);

    land.setMinDiff(nmin, ndiff);

    ui->complexcanvas->drawLandscape(land);

    ui->statusBar->setLandscape(ui->complexcanvas->getReference());

    add(land);
}

void MainWindow::centre()
{
    Landscape land = ui->complexcanvas->getLandscape();

    land.setMinDiff(-land.getDiff()/2.0, land.getDiff());

    ui->complexcanvas->drawLandscape(land);

    ui->statusBar->setLandscape(ui->complexcanvas->getReference());

    add(land);
}

void MainWindow::setEnabled(bool b)
{
    ui->mainToolBar->setDisabled(b);
}

void MainWindow::add(const Landscape &land)
{
    _history.add(land);
}

void MainWindow::undo()
{
    if (!_history.isAtBottom())
    {
        auto un = _history.undo();
        ui->complexcanvas->drawLandscape(un);
        ui->statusBar->setLandscape(ui->complexcanvas->getReference());
    }
}

void MainWindow::redo()
{
    if (!_history.isAtTop())
    {
        auto un = _history.redo();
        ui->complexcanvas->drawLandscape(un);
        ui->statusBar->setLandscape(ui->complexcanvas->getReference());
    }
}

void MainWindow::history()
{


    _historydialog->setHistory(_history);

    if (_historydialog->exec() == QDialog::Accepted)
    {
        ui->complexcanvas->drawLandscape(_history.revert(_historydialog->getIndex()));
        ui->statusBar->setLandscape(ui->complexcanvas->getReference());
    }
}

void MainWindow::calculator()
{
    _calculatordialog->exec();
}

void MainWindow::setup(bool fp64)
{
    ui->complexcanvas->setup(fp64);
}

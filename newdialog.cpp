#include "newdialog.h"
#include "ui_newdialog.h"

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);

    setModal(true);
}

NewDialog::~NewDialog()
{
    delete ui;
}

void NewDialog::accept()
{
    Landscape newland = _land;

    if (ui->txtMin->text() == "" || ui->txtMax->text() == "")
    {
        QMessageBox::warning(this, "ComplexCL", "Please enter a formula into all three boxes", QMessageBox::Ok);
        return;
    }

    try
    {
        newland.setDomain(ui->txtMin->text(), ui->txtMax->text());
        newland.setString(ui->txtFormula->toPlainText());
    }
    catch (Evaluator::EvaluatorParseException &e)
    {
        QMessageBox::warning(this, "ComplexCL", "Formula parse error - " + QString(e.what()), QMessageBox::Ok);
        return;
    }

    _land = newland;

    QDialog::accept();

}

int NewDialog::exec()
{
    ui->txtFormula->setText(_land.getFormula());
    ui->txtMin->setText(toString(_land.getMin(), 10));
    ui->txtMax->setText(toString(_land.getMax(), 10));

    return QDialog::exec();
}

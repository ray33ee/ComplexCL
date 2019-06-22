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
    if (ui->txtMin->text() == "" || ui->txtMax->text() == "")
    {
        QMessageBox::warning(this, "ComplexCL", "Please enter a formula into all three boxes", QMessageBox::Ok);
        return;
    }

    Evaluator min;
    Evaluator max;

    try
    {
        min.setString(ui->txtMin->text());
        max.setString(ui->txtMax->text());
        _function.setString(ui->txtFormula->toPlainText());

    }
    catch (Evaluator::EvaluatorParseException e)
    {
        QMessageBox::warning(this, "ComplexCL", "Formula parse error - " + QString(e.what()), QMessageBox::Ok);
        return;
    }

    _min = min(0.0);
    _max = max(0.0);


    QDialog::accept();

}

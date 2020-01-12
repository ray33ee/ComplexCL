#include "calculatordialog.h"
#include "ui_calculatordialog.h"

CalculatorDialog::CalculatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalculatorDialog)
{
    ui->setupUi(this);

    ui->txtAnswer->setReadOnly(true);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Calculate");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Ok");

    accept();

    setModal(true);

}

void CalculatorDialog::accept()
{
    try
    {
        Evaluator z(ui->txtInput->text());

        //QString func = ui->txtFormula->toPlainText().replace("ans", QString() + "(" + std::toString(Evaluator(ui->txtAnswer->text())(0.0), 20) + ")", Qt::CaseInsensitive);

        QString func = ui->txtFormula->toPlainText();

        Evaluator functor(func);

        auto val = z(0.0);

        auto result = functor(val);


        ui->txtAnswer->setText(std::toString(result, 10));

    }
    catch (std::exception e)
    {
        QMessageBox::warning(this, "ComplexCL", "Formula parse error - " + QString(e.what()), QMessageBox::Ok);
    }
}

CalculatorDialog::~CalculatorDialog()
{
    delete ui;
}

#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <evaluator.h>

namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent = nullptr);
    ~NewDialog();

    std::complex<double> getMin() const { return _min; }
    std::complex<double> getMax() const { return _max; }

    Evaluator getEvaluator() const { return _function; }

private:
    Ui::NewDialog *ui;

    Evaluator _function;

    std::complex<double> _min;
    std::complex<double> _max;

public slots:
    void accept();

};

#endif // NEWDIALOG_H

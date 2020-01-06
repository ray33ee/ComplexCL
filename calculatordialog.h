#ifndef CALCULATORDIALOG_H
#define CALCULATORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "landscape.h"
#include <QMessageBox>

#ifdef QT_DEBUG
#include "qdebug.h"
#endif

namespace Ui {
class CalculatorDialog;
}

class CalculatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalculatorDialog(QWidget *parent = nullptr);
    ~CalculatorDialog();

private:
    Ui::CalculatorDialog *ui;

public slots:

    virtual void accept();



};

#endif // CALCULATORDIALOG_H

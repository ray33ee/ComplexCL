#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include "history.h"
#include "landscape.h"

namespace Ui {
class HistoryDialog;
}

class HistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryDialog(QWidget *parent = nullptr);
    ~HistoryDialog();

    void setHistory(const History<Landscape> &);

    int getIndex() const;

private:
    Ui::HistoryDialog *ui;

public slots:

};

#endif // HISTORYDIALOG_H

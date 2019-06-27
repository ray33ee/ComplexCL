#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <landscape.h>

namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent = nullptr);
    ~NewDialog();

    const Landscape& getLandscape() const { return _land; }

    void setLandscape(const Landscape &land) { _land = land; }

private:
    Ui::NewDialog *ui;

    Landscape _land;



public slots:
    void accept();

    virtual int exec();

};

#endif // NEWDIALOG_H

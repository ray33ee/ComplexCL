#include "historydialog.h"
#include "ui_historydialog.h"

HistoryDialog::HistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistoryDialog)
{
    ui->setupUi(this);

    setModal(true);
}

void HistoryDialog::setHistory(const History<Landscape> &his)
{
    auto history = his.getArray();

    ui->listWidget->clear();

    for (const auto &land : history)
        ui->listWidget->addItem(land.toString());

    ui->listWidget->setCurrentRow(his.getPointer() - 1);
}

int HistoryDialog::getIndex() const
{
    return ui->listWidget->currentRow();
}

HistoryDialog::~HistoryDialog()
{
    delete ui;
}


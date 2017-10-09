#ifndef TABLEOFFERSDIALOG_H
#define TABLEOFFERSDIALOG_H

#include <QDialog>

namespace Ui {
    class TableOffersDialog;
}

class TableOffersDialog : public QDialog
{
    Q_OBJECT

public:
    TableOffersDialog(QDialog *parent = nullptr);
    ~TableOffersDialog();

private:
    Ui::TableOffersDialog *ui;
};

#endif

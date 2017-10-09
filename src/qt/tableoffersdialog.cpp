#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(QDialog *parent) : QDialog(parent), ui(new Ui::TableOffersDialog)
{
    ui->setupUi(this);
}

TableOffersDialog::~TableOffersDialog()
{
    delete ui;
}

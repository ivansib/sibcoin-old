#include "exchangedialog.h"
#include "ui_exchangedialog.h"


ExchangeDialog::ExchangeDialog(QDialog *parent) : QDialog(parent), ui(new Ui::ExchangeDialog)
{
    ui->setupUi(this);
}

ExchangeDialog::~ExchangeDialog()
{

}

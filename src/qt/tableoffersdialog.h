#ifndef TABLEOFFERSDIALOG_H
#define TABLEOFFERSDIALOG_H

#include <QDialog>
#include "dex/dexdb.h"

using namespace dex;

namespace Ui {
    class TableOffersDialog;
}

class TableOffersDialog : public QDialog
{
    Q_OBJECT

public:
    TableOffersDialog(const std::map<unsigned char, PaymentMethodInfo> &payments,
                      const std::map<std::string, CountryInfo> &countries,
                      const std::map<std::string, CurrencyInfo> &currencies, QDialog *parent = nullptr);
    ~TableOffersDialog();

private:
    Ui::TableOffersDialog *ui;
};

#endif

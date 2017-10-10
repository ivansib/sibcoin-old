#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(const std::map<unsigned char, PaymentMethodInfo> &payments,
                                     const std::map<std::string, CountryInfo> &countries,
                                     const std::map<std::string, CurrencyInfo> &currencies, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog)
{
    ui->setupUi(this);

    auto itPayment = payments.begin();
    while (itPayment != payments.end()) {
        ui->cBoxPayment->addItem(tr(itPayment->second.name.c_str()), itPayment->first);
        ++itPayment;
    }

    auto itCountry = countries.begin();
    while (itCountry != countries.end()) {
        ui->cBoxCountry->addItem(tr(itCountry->second.name.c_str()), QString::fromUtf8(itCountry->first.c_str()));
        ++itCountry;
    }

    auto itCurrency = currencies.begin();
    while (itCurrency != currencies.end()) {
        ui->cBoxCurrency->addItem(tr(itCurrency->second.name.c_str()), QString::fromUtf8(itCurrency->first.c_str()));
        ++itCurrency;
    }
}

TableOffersDialog::~TableOffersDialog()
{
    delete ui;
}

#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"
#include "offermodel.h"

TableOffersDialog::TableOffersDialog(DexDB *db, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog), db(db)
{
    ui->setupUi(this);


    OfferModel *pModel = new OfferModel(db->getOffersBuy());
    ui->tableView->setModel(pModel);

    auto payments = db->getPaymentMethodsInfo();
    auto itPayment = payments.begin();
    while (itPayment != payments.end()) {
        ui->cBoxPayment->addItem(tr(itPayment->second.name.c_str()), itPayment->first);
        ++itPayment;
    }

    auto countries = db->getCountriesInfo();
    auto itCountry = countries.begin();
    while (itCountry != countries.end()) {
        ui->cBoxCountry->addItem(tr(itCountry->second.name.c_str()), QString::fromUtf8(itCountry->first.c_str()));
        ++itCountry;
    }

    auto currencies = db->getCurrenciesInfo();
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

#include "offerdetailseditor.h"
#include "ui_offerdetailseditor.h"

OfferDetailsEditor::OfferDetailsEditor(DexDB *db, QDialog *parent) : QDialog(parent), ui(new Ui::OfferDetailsEditor)
{
    ui->setupUi(this);

    auto payments = db->getPaymentMethodsInfo();
    ui->cBoxPayment->addData(payments);

    auto countries = db->getCountriesInfo();
    ui->cBoxCountry->addData(countries);

    auto currencies = db->getCurrenciesInfo();
    ui->cBoxCurrency->addData(currencies);
}

OfferDetailsEditor::~OfferDetailsEditor()
{
    delete ui;
}

void OfferDetailsEditor::setOfferInfo(const QtOfferInfo &info)
{

}

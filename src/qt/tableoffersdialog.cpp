#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(DexDB *db, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog), db(db)
{
    ui->setupUi(this);


    pModel = new OfferModel(db->getOffersBuy());
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

    connect(ui->cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCountryIso);
    connect(ui->cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCurrencyIso);
    connect(ui->cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterPaymentMethod);
}

TableOffersDialog::~TableOffersDialog()
{
    delete ui;
}

void TableOffersDialog::changedFilterCountryIso(const int &) {
    std::string iso = ui->cBoxCountry->currentData().toString().toUtf8().constData();
    pModel->setFilterCountryIso(iso);
}

void TableOffersDialog::changedFilterCurrencyIso(const int &) {
    std::string iso = ui->cBoxCurrency->currentData().toString().toUtf8().constData();
    pModel->setFilterCurrencyIso(iso);
}

void TableOffersDialog::changedFilterPaymentMethod(const int &)
{
    uint8_t payment = ui->cBoxPayment->currentData().toInt();
    pModel->setFilterPaymentMethod(payment);
}

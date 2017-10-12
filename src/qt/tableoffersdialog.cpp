#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(DexDB *db, const TypeOffer &type, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog), db(db)
{
    ui->setupUi(this);

    if (type == Buy) {
        pModel = new OfferModel(db->getOffersBuy());
    } else {
        pModel = new OfferModel(db->getOffersSell());
    }

    ui->tableView->setModel(pModel);

    initComboPayment();
    initComboCountry();
    initComboCurrency();

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

void TableOffersDialog::initComboPayment()
{
    auto payments = db->getPaymentMethodsInfo();
    auto itPayment = payments.begin();
    while (itPayment != payments.end()) {
        ui->cBoxPayment->addItem(tr(itPayment->second.name.c_str()), itPayment->first);
        ++itPayment;
    }
}

void TableOffersDialog::initComboCountry() {
    auto countries = db->getCountriesInfo();
    auto itCountry = countries.begin();

    QMap<QString, QString> countrySort;
    while (itCountry != countries.end()) {
        countrySort[tr(itCountry->second.name.c_str())] = QString::fromUtf8(itCountry->first.c_str());
        ++itCountry;
    }

    auto itCountrySort = countrySort.begin();
    while (itCountrySort != countrySort.end()) {
        ui->cBoxCountry->addItem(itCountrySort.key(), itCountrySort.value());
        ++itCountrySort;
    }
}

void TableOffersDialog::initComboCurrency()
{
    auto currencies = db->getCurrenciesInfo();
    auto itCurrency = currencies.begin();

    QMap<QString, QString> currencySort;
    while (itCurrency != currencies.end()) {
        currencySort[tr(itCurrency->second.name.c_str())] = QString::fromUtf8(itCurrency->first.c_str());
        ++itCurrency;
    }

    auto itCurrencySort = currencySort.begin();
    while (itCurrencySort != currencySort.end()) {
        ui->cBoxCurrency->addItem(itCurrencySort.key(), itCurrencySort.value());
        ++itCurrencySort;
    }
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

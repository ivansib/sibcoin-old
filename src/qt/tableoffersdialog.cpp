#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(DexDB *db, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog), db(db)
{
    ui->setupUi(this);

    pModel = new OfferModel();

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setModel(pModel);

    auto payments = db->getPaymentMethodsInfo();
    ui->cBoxPayment->addData(payments);

    auto countries = db->getCountriesInfo();
    ui->cBoxCountry->addData(countries);

    auto currencies = db->getCurrenciesInfo(DexDB::Enabled);
    ui->cBoxCurrency->addData(currencies);

    connect(ui->cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCountryIso);
    connect(ui->cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCurrencyIso);
    connect(ui->cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterPaymentMethod);

    connect(ui->tableView, &QTableView::doubleClicked, this, &TableOffersDialog::clickedColumn);
}

TableOffersDialog::~TableOffersDialog()
{
    delete ui;
}

void TableOffersDialog::init()
{
    changedFilterCountryIso(0);
    changedFilterCurrencyIso(0);
    changedFilterPaymentMethod(0);
}

QString TableOffersDialog::currentCountry() const
{
    return ui->cBoxCountry->currentText();
}

QString TableOffersDialog::currentCurrency() const
{
    return ui->cBoxCurrency->currentText();
}

QString TableOffersDialog::currentPayment() const
{
    return ui->cBoxPayment->currentText();
}

void TableOffersDialog::changedFilterCountryIso(const int &) {
    QString iso = ui->cBoxCountry->currentData().toString();
    pModel->setFilterCountryIso(iso);
}

void TableOffersDialog::changedFilterCurrencyIso(const int &) {
    QString iso = ui->cBoxCurrency->currentData().toString();
    pModel->setFilterCurrencyIso(iso);
}

void TableOffersDialog::changedFilterPaymentMethod(const int &)
{
    quint8 payment = ui->cBoxPayment->currentData().toInt();
    pModel->setFilterPaymentMethod(payment);
}

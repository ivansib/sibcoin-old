#include <QPushButton>
#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(DexDB *db, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog), db(db)
{
    ui->setupUi(this);

    pModel = new OfferModel();
    pMapper = new QSignalMapper(this);

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setModel(pModel);

    updateNavigationData();
    resizeColumns();

    connect(ui->cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCountryIso);
    connect(ui->cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCurrencyIso);
    connect(ui->cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterPaymentMethod);

    connect(pModel, &OfferModel::layoutChanged, this, &TableOffersDialog::addButtons);

    useMyOfferMode(false);
}

TableOffersDialog::~TableOffersDialog()
{
    delete ui;
}

void TableOffersDialog::updateNavigationData()
{
    auto payments = db->getPaymentMethodsInfo();
    ui->cBoxPayment->addData(payments, ComboBox::View);

    auto countries = db->getCountriesInfo();
    ui->cBoxCountry->addData(countries, ComboBox::View);

    auto currencies = db->getCurrenciesInfo();
    ui->cBoxCurrency->addData(currencies, ComboBox::View);

    ui->cBoxOffer->clear();
    ui->cBoxOffer->addItem(tr("All"));
    ui->cBoxOffer->addItem(tr("Buy"));
    ui->cBoxOffer->addItem(tr("Sell"));

    Q_EMIT navigationDataUpdate();
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

int TableOffersDialog::currentOfferIndex() const
{
    return ui->cBoxOffer->currentIndex();
}

void TableOffersDialog::useMyOfferMode(const bool &b)
{
    ui->cBoxOffer->setVisible(b);
    ui->labelOffer->setVisible(b);
    ui->widgetBottom->setVisible(b);

    if (b) {
        connect(ui->cBoxOffer, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &TableOffersDialog::changedFilterOfferType);

        connect(ui->btnCreate, &QPushButton::clicked, this, &TableOffersDialog::openCreatorOffer);
    }
}

void TableOffersDialog::addButtons()
{
    for (int i = 0; i < pModel->rowCount(); i++) {
        QPushButton *pBtn = new QPushButton(tr("Show"));
        ui->tableView->setIndexWidget(pModel->index(i, 3), pBtn);
        connect(pBtn, SIGNAL(clicked()), pMapper, SLOT(map()));
        pMapper->setMapping(pBtn, i);
    }

    connect(pMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &TableOffersDialog::clickedButton);
}

void TableOffersDialog::resizeColumns()
{
    int defSize = ui->tableView->columnWidth(0);

    ui->tableView->setColumnWidth(1, defSize * 2);
    ui->tableView->setColumnWidth(3, defSize * 1.2);
}

void TableOffersDialog::changedFilterOfferType(const int &)
{
    int type = ui->cBoxOffer->currentIndex();
    pModel->setFilterTypeOffer(type);
}

void TableOffersDialog::openCreatorOffer()
{
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

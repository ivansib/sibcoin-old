#include "exchangedialog.h"
#include "ui_exchangedialog.h"
#include "util.h"

ExchangeDialog::ExchangeDialog(QDialog *parent) : QDialog(parent), ui(new Ui::ExchangeDialog), db(strDexDbFile)
{
    ui->setupUi(this);

    auto payments = db.getPaymentMethodsInfo();
    auto countries = db.getCountriesInfo();
    auto currencies = db.getCurrenciesInfo();

    tableBuy = new TableOffersDialog(payments, countries, currencies);
    tableSell = new TableOffersDialog(payments, countries, currencies);
    widgetMyOffers = new QWidget();
    widgetExchanges = new QWidget();
    widgetSettings = new QWidget();

    ui->stackedWidget->addWidget(tableBuy);
    ui->stackedWidget->addWidget(tableSell);
    ui->stackedWidget->addWidget(widgetMyOffers);
    ui->stackedWidget->addWidget(widgetExchanges);
    ui->stackedWidget->addWidget(widgetSettings);

    connect(ui->btnBuy, &QPushButton::clicked, this, &ExchangeDialog::currentPageBuy);
    connect(ui->btnSell, &QPushButton::clicked, this, &ExchangeDialog::currentPageSell);
    connect(ui->btnMyOffers, &QPushButton::clicked, this, &ExchangeDialog::currentPageMyOffers);
    connect(ui->btnExchanges, &QPushButton::clicked, this, &ExchangeDialog::currentPageExchanges);
    connect(ui->btnSettings, &QPushButton::clicked, this, &ExchangeDialog::currentPageSettings);
}

ExchangeDialog::~ExchangeDialog()
{
    delete tableBuy;
    delete tableSell;
    delete widgetMyOffers;
    delete widgetExchanges;
    delete widgetSettings;
    delete ui;
}

void ExchangeDialog::currentPageBuy()
{
    ui->stackedWidget->setCurrentWidget(tableBuy);
}

void ExchangeDialog::currentPageSell()
{
    ui->stackedWidget->setCurrentWidget(tableSell);
}

void ExchangeDialog::currentPageMyOffers()
{
    ui->stackedWidget->setCurrentWidget(widgetMyOffers);
}

void ExchangeDialog::currentPageExchanges()
{
    ui->stackedWidget->setCurrentWidget(widgetExchanges);
}

void ExchangeDialog::currentPageSettings()
{
    ui->stackedWidget->setCurrentWidget(widgetSettings);
}

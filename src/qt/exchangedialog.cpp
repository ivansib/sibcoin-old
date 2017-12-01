#include "exchangedialog.h"
#include "ui_exchangedialog.h"
#include "util.h"

ExchangeDialog::ExchangeDialog(QDialog *parent) : QDialog(parent), ui(new Ui::ExchangeDialog)
{
    ui->setupUi(this);

    callback = new CallBackDbForGui();
    connect(callback, &CallBackDbForGui::tableOperationFinished, this, &ExchangeDialog::finishTableOperation);

    db = new DexDB(strDexDbFile, callback);

    tableBuy = new TableOffersView(db, TableOffersView::Buy);
    tableSell = new TableOffersView(db, TableOffersView::Sell);
    tableMyOffers = new TableOffersEditor(db);
    widgetExchanges = new QWidget();
    settings = new SettingsDialog(db);

    ui->stackedWidget->addWidget(tableBuy);
    ui->stackedWidget->addWidget(tableSell);
    ui->stackedWidget->addWidget(tableMyOffers);
    ui->stackedWidget->addWidget(widgetExchanges);
    ui->stackedWidget->addWidget(settings);

    connect(ui->btnBuy, &QPushButton::clicked, this, &ExchangeDialog::currentPageBuy);
    connect(ui->btnSell, &QPushButton::clicked, this, &ExchangeDialog::currentPageSell);
    connect(ui->btnMyOffers, &QPushButton::clicked, this, &ExchangeDialog::currentPageMyOffers);
    connect(ui->btnExchanges, &QPushButton::clicked, this, &ExchangeDialog::currentPageExchanges);
    connect(ui->btnSettings, &QPushButton::clicked, this, &ExchangeDialog::currentPageSettings);

    connect(settings, &SettingsDialog::dataChanged, this, &ExchangeDialog::updateData);
}

ExchangeDialog::~ExchangeDialog()
{
    delete tableBuy;
    delete tableSell;
    delete tableMyOffers;
    delete widgetExchanges;
    delete settings;
    delete db;
    delete callback;
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
    ui->stackedWidget->setCurrentWidget(tableMyOffers);
}

void ExchangeDialog::currentPageExchanges()
{
    ui->stackedWidget->setCurrentWidget(widgetExchanges);
}

void ExchangeDialog::currentPageSettings()
{
    ui->stackedWidget->setCurrentWidget(settings);
}

void ExchangeDialog::updateData()
{
    tableBuy->updateNavigationData();
    tableSell->updateNavigationData();
    tableMyOffers->updateNavigationData();
}

void ExchangeDialog::finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
}

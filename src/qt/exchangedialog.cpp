#include "exchangedialog.h"
#include "ui_exchangedialog.h"
#include "util.h"

ExchangeDialog::ExchangeDialog(QDialog *parent) : QDialog(parent), ui(new Ui::ExchangeDialog)
{
    ui->setupUi(this);

    callback = CallBackDbForGui::instance();
    connect(callback, SIGNAL(tableOperationFinished(TypeTable, TypeTableOperation, StatusTableOperation)),
            this, SLOT(finishTableOperation(TypeTable, TypeTableOperation, StatusTableOperation)));

    db = DexDB::instance();
    db->addCallBack(callback);

    tableBuy = new TableOffersView(db, TableOffersView::Buy, CommonSettingsForOffers::BuyOffer);
    tableSell = new TableOffersView(db, TableOffersView::Sell, CommonSettingsForOffers::SellOffer);
    tableMyOffers = new TableOffersEditor(db);
    widgetExchanges = new QWidget();
    settings = new SettingsDialog(db);

    ui->btnExchanges->setVisible(false);

    ui->stackedWidget->addWidget(tableBuy);
    ui->stackedWidget->addWidget(tableSell);
    ui->stackedWidget->addWidget(tableMyOffers);
    ui->stackedWidget->addWidget(widgetExchanges);
    ui->stackedWidget->addWidget(settings);

    connect(ui->btnBuy, SIGNAL(clicked()), this, SLOT(currentPageBuy()));
    connect(ui->btnSell, SIGNAL(clicked()), this, SLOT(currentPageSell()));
    connect(ui->btnMyOffers, SIGNAL(clicked()), this, SLOT(currentPageMyOffers()));
    connect(ui->btnExchanges, SIGNAL(clicked()), this, SLOT(currentPageExchanges()));
    connect(ui->btnSettings, SIGNAL(clicked()), this, SLOT(currentPageSettings()));

    connect(settings, SIGNAL(dataChanged()), tableBuy, SLOT(updateData()));
    connect(settings, SIGNAL(dataChanged()), tableSell, SLOT(updateData()));
    connect(settings, SIGNAL(dataChanged()), tableMyOffers, SLOT(updateData()));

    currentPageBuy();
    ui->btnBuy->setChecked(true);
}

ExchangeDialog::~ExchangeDialog()
{
    delete tableBuy;
    delete tableSell;
    delete tableMyOffers;
    delete widgetExchanges;
    delete settings;

    db->removeCallBack(callback);
    callback->freeInstance();
    db->freeInstance();

    delete ui;
}

void ExchangeDialog::setModel(WalletModel *model)
{
    this->model = model;
    tableMyOffers->setModel(model);
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

void ExchangeDialog::finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
}

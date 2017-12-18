#include <QPushButton>
#include <QScrollBar>
#include "tableoffersdialog.h"
#include "ui_tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(DexDB *db, const OfferModel::TypeTable &typeTable, QDialog *parent) :
    QDialog(parent), ui(new Ui::TableOffersDialog), db(db)
{
    ui->setupUi(this);

    callBack = static_cast<CallBackDbForGui *>(db->getCallBack());

    if (callBack != nullptr) {
        connect(callBack, &CallBackDbForGui::tableOperationFinished, this, &TableOffersDialog::updateTables);
    }

    pModel = new OfferModel(typeTable);
    pDelegate = new TableOfferDelegate();

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setModel(pModel);
    ui->tableView->setItemDelegate(pDelegate);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    ui->tableView->setColumnWidth(0, 150);
    ui->tableView->setColumnWidth(2, 150);
    ui->tableView->setColumnWidth(3, 150);
    ui->tableView->setColumnWidth(4, 150);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(ui->tableView, 120, 23);

    updateNavigationData();

    connect(ui->cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCountryIso);
    connect(ui->cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCurrencyIso);
    connect(ui->cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterPaymentMethod);

    connect(pDelegate, &TableOfferDelegate::clicked, this, &TableOffersDialog::clickedButton);

    useMyOfferMode(false);
}

TableOffersDialog::~TableOffersDialog()
{
    delete pDelegate;
    delete pModel;
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

void TableOffersDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    columnResizingFixer->stretchColumnWidth(1);
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

void TableOffersDialog::changedFilterOfferType(const int &)
{
    int type = ui->cBoxOffer->currentIndex();
    pModel->setFilterTypeOffer(type);
}

void TableOffersDialog::openCreatorOffer()
{
}

void TableOffersDialog::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
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

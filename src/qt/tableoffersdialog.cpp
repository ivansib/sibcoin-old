#include <QPushButton>
#include <QScrollBar>
#include "tableoffersdialog.h"

TableOffersDialog::TableOffersDialog(DexDB *db, const OfferModel::TypeTable &typeTable, QDialog *parent) :
    QDialog(parent), db(db)
{
    setupUi(this);

    callBack = static_cast<CallBackDbForGui *>(db->getCallBack());

    if (callBack != nullptr) {
        connect(callBack, &CallBackDbForGui::tableOperationFinished, this, &TableOffersDialog::updateTables);
    }

    pModel = new OfferModel(typeTable);
    pDelegate = new TableOfferDelegate();

    tableView->setSortingEnabled(true);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setModel(pModel);
    tableView->setItemDelegate(pDelegate);
    tableView->setAlternatingRowColors(true);
    tableView->verticalHeader()->hide();
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 150);
    tableView->setColumnWidth(4, 150);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(tableView, 120, 23);

    updateNavigationData();

    connect(cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCountryIso);
    connect(cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterCurrencyIso);
    connect(cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersDialog::changedFilterPaymentMethod);

    connect(pDelegate, &TableOfferDelegate::clicked, this, &TableOffersDialog::clickedButton);

    useMyOfferMode(false);
}

TableOffersDialog::~TableOffersDialog()
{
    delete pDelegate;
    delete pModel;
}

void TableOffersDialog::updateNavigationData()
{
    auto payments = db->getPaymentMethodsInfo();
    cBoxPayment->addData(payments, ComboBox::View);

    auto countries = db->getCountriesInfo();
    cBoxCountry->addData(countries, ComboBox::View);

    auto currencies = db->getCurrenciesInfo();
    cBoxCurrency->addData(currencies, ComboBox::View);

    cBoxOffer->clear();
    cBoxOffer->addItem(tr("All"));
    cBoxOffer->addItem(tr("Buy"));
    cBoxOffer->addItem(tr("Sell"));

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
    return cBoxCountry->currentText();
}

QString TableOffersDialog::currentCurrency() const
{
    return cBoxCurrency->currentText();
}

QString TableOffersDialog::currentPayment() const
{
    return cBoxPayment->currentText();
}

int TableOffersDialog::currentOfferIndex() const
{
    return cBoxOffer->currentIndex();
}

void TableOffersDialog::useMyOfferMode(const bool &b)
{
    cBoxOffer->setVisible(b);
    labelOffer->setVisible(b);
    widgetBottom->setVisible(b);

    if (b) {
        connect(cBoxOffer, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &TableOffersDialog::changedFilterOfferType);

        connect(btnCreate, &QPushButton::clicked, this, &TableOffersDialog::openCreatorOffer);
    }
}

void TableOffersDialog::changedFilterOfferType(const int &)
{
    int type = cBoxOffer->currentIndex();
    pModel->setFilterTypeOffer(type);
}

void TableOffersDialog::openCreatorOffer()
{
}

void TableOffersDialog::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
}

void TableOffersDialog::changedFilterCountryIso(const int &) {
    QString iso = cBoxCountry->currentData().toString();
    pModel->setFilterCountryIso(iso);
}

void TableOffersDialog::changedFilterCurrencyIso(const int &) {
    QString iso = cBoxCurrency->currentData().toString();
    pModel->setFilterCurrencyIso(iso);
}

void TableOffersDialog::changedFilterPaymentMethod(const int &)
{
    quint8 payment = cBoxPayment->currentData().toInt();
    pModel->setFilterPaymentMethod(payment);
}

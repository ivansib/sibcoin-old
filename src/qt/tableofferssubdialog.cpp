#include <QPushButton>
#include <QScrollBar>
#include "tableofferssubdialog.h"

TableOffersSubDialog::TableOffersSubDialog(DexDB *db, const int &columnBtn, const CommonSettingsForOffers::TypeSettings &typeSettings, QDialog *parent) :
    QDialog(parent), db(db), typeSettings(typeSettings)
{
    setupUi(this);

    callBack = CallBackDbForGui::instance();
    db->addCallBack(callBack);

    connect(callBack, &CallBackDbForGui::tableOperationFinished, this, &TableOffersSubDialog::updateTables);

    settings = CommonSettingsForOffers::instance();
    pDelegate = new TableOfferDelegate(columnBtn);

    tableView->setSortingEnabled(true);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setItemDelegate(pDelegate);
    tableView->setAlternatingRowColors(true);
    tableView->verticalHeader()->hide();
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    updateNavigationData();

    connect(cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersSubDialog::changedFilterCountryIso);
    connect(cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersSubDialog::changedFilterCurrencyIso);
    connect(cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &TableOffersSubDialog::changedFilterPaymentMethod);

    connect(pDelegate, &TableOfferDelegate::clicked, this, &TableOffersSubDialog::clickedButton);

    useMyOfferMode(false);
}

TableOffersSubDialog::~TableOffersSubDialog()
{
    delete pDelegate;
    settings->freeInstance();
    db->removeCallBack(callBack);
    callBack->freeInstance();
}

void TableOffersSubDialog::updateNavigationData()
{
    auto payments = db->getPaymentMethodsInfo();
    cBoxPayment->addData(payments, ComboBox::View);

    auto countries = db->getCountriesInfo();
    cBoxCountry->addData(countries, ComboBox::View);

    auto currencies = db->getCurrenciesInfo();
    cBoxCurrency->addData(currencies, ComboBox::View);

    cBoxPayment->setCurrentData(QString::number(settings->getPaymentMethodType(typeSettings)));
    cBoxCountry->setCurrentData(settings->getCountryIso(typeSettings));
    cBoxCurrency->setCurrentData(settings->getCurrencyIso(typeSettings));

    cBoxOffer->clear();
    cBoxOffer->addItem(tr("All"));
    cBoxOffer->addItem(tr("Buy"));
    cBoxOffer->addItem(tr("Sell"));

    cBoxOffer->setCurrentIndex(settings->getTypeOffer(typeSettings));

    Q_EMIT navigationDataUpdate();
}

void TableOffersSubDialog::init()
{
    changedFilterCountryIso(0);
    changedFilterCurrencyIso(0);
    changedFilterPaymentMethod(0);
    changedFilterOfferType(0);
}

QString TableOffersSubDialog::currentCountry() const
{
    return cBoxCountry->currentText();
}

QString TableOffersSubDialog::currentCurrency() const
{
    return cBoxCurrency->currentText();
}

QString TableOffersSubDialog::currentPayment() const
{
    return cBoxPayment->currentText();
}

int TableOffersSubDialog::currentOfferIndex() const
{
    return cBoxOffer->currentIndex();
}

void TableOffersSubDialog::useMyOfferMode(const bool &b)
{
    cBoxOffer->setVisible(b);
    labelOffer->setVisible(b);
    widgetBottom->setVisible(b);

    if (b) {
        connect(cBoxOffer, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &TableOffersSubDialog::changedFilterOfferType);

        connect(btnCreate, &QPushButton::clicked, this, &TableOffersSubDialog::openCreatorOffer);
    }
}

void TableOffersSubDialog::openCreatorOffer()
{
}

void TableOffersSubDialog::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
}

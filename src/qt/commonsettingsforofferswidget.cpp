#include "commonsettingsforofferswidget.h"

CommonSettingsForOffersWidget::CommonSettingsForOffersWidget(DexDB *db, QWidget *parent)
    : QWidget(parent), db(db)
{
    setupUi(this);

    settings = CommonSettingsForOffers::instance();

    cBoxCountry->addData(db->getCountriesInfo(), ComboBox::Editor);
    cBoxCurrency->addData(db->getCurrenciesInfo(), ComboBox::Editor);
    cBoxPayment->addData(db->getPaymentMethodsInfo(), ComboBox::Editor);

    setData();

    connectWidgets();

    connect(chBoxMaxRows, SIGNAL(toggled(bool)), this, SLOT(changedShowMaxRows(bool)));
}

CommonSettingsForOffersWidget::~CommonSettingsForOffersWidget()
{
    settings->freeInstance();
}

void CommonSettingsForOffersWidget::update()
{
    setData();
}

void CommonSettingsForOffersWidget::saveData()
{
    settings->setCountryIso(CommonSettingsForOffers::EditOffer, cBoxCountry->currentData().toString());
    settings->setCurrencyIso(CommonSettingsForOffers::EditOffer, cBoxCurrency->currentData().toString());
    settings->setPaymentMethodType(CommonSettingsForOffers::EditOffer, cBoxPayment->currentData().toReal());
    settings->setMinAmount(sEditMinAmount->value());
    settings->setShowMaxRowsTables(chBoxMaxRows->isChecked());
    settings->setNumRowsTables(sBoxNumRows->value());
}

void CommonSettingsForOffersWidget::cancel()
{
    setData();
}

void CommonSettingsForOffersWidget::setData()
{
    disconnectWidgets();
    settings->sync();
    cBoxCountry->setCurrentData(settings->getCountryIso(CommonSettingsForOffers::EditOffer));
    cBoxCurrency->setCurrentData(settings->getCurrencyIso(CommonSettingsForOffers::EditOffer));
    cBoxPayment->setCurrentData(QString::number(settings->getPaymentMethodType(CommonSettingsForOffers::EditOffer)));
    sEditMinAmount->setValue(settings->getMinAmount());
    chBoxMaxRows->setChecked(settings->getShowMaxRowsTables());
    sBoxNumRows->setValue(settings->getNumRowsTables());

    changedShowMaxRows(settings->getShowMaxRowsTables());

    connectWidgets();
}

void CommonSettingsForOffersWidget::connectWidgets()
{
    connect(cBoxCountry, SIGNAL(currentIndexChanged(int)), this, SLOT(changedData(int)));
    connect(cBoxCurrency, SIGNAL(currentIndexChanged(int)), this, SLOT(changedData(int)));
    connect(cBoxPayment, SIGNAL(currentIndexChanged(int)), this, SLOT(changedData(int)));
    connect(sEditMinAmount, SIGNAL(valueChanged(quint64)), this, SLOT(changedData(quint64)));
    connect(chBoxMaxRows, SIGNAL(clicked()), this, SLOT(changedData()));
    connect(sBoxNumRows, SIGNAL(valueChanged(int)), this, SLOT(changedData(int)));
}

void CommonSettingsForOffersWidget::disconnectWidgets()
{
    disconnect(cBoxCountry, SIGNAL(currentIndexChanged(int)), this, SLOT(changedData(int)));
    disconnect(cBoxCurrency, SIGNAL(currentIndexChanged(int)), this, SLOT(changedData(int)));
    disconnect(cBoxPayment, SIGNAL(currentIndexChanged(int)), this, SLOT(changedData(int)));
    disconnect(sEditMinAmount, SIGNAL(valueChanged(quint64)), this, SLOT(changedData(quint64)));
    disconnect(chBoxMaxRows, SIGNAL(clicked()), this, SLOT(changedData()));
    disconnect(sBoxNumRows, SIGNAL(valueChanged(int)), this, SLOT(changedData(int)));
}

void CommonSettingsForOffersWidget::changedData()
{
    Q_EMIT dataChanged();
}

void CommonSettingsForOffersWidget::changedData(int)
{
    Q_EMIT dataChanged();
}

void CommonSettingsForOffersWidget::changedData(quint64)
{
    Q_EMIT dataChanged();
}

void CommonSettingsForOffersWidget::changedShowMaxRows(bool isCheked)
{
    sBoxNumRows->setEnabled(!isCheked);
}

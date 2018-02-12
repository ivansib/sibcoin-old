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

    connect(cBoxCountry, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CommonSettingsForOffersWidget::changedData);
    connect(cBoxCurrency, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CommonSettingsForOffersWidget::changedData);
    connect(cBoxPayment, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CommonSettingsForOffersWidget::changedData);
    connect(sEditMinAmount, &PriceSpinBox::valueChanged, this, &CommonSettingsForOffersWidget::changedData);
}

CommonSettingsForOffersWidget::~CommonSettingsForOffersWidget()
{
    settings->freeInstance();
}

void CommonSettingsForOffersWidget::saveData()
{
    settings->setCountryIso(CommonSettingsForOffers::EditOffer, cBoxCountry->currentData().toString());
    settings->setCurrencyIso(CommonSettingsForOffers::EditOffer, cBoxCurrency->currentData().toString());
    settings->setPaymentMethodType(CommonSettingsForOffers::EditOffer, cBoxPayment->currentData().toReal());
    settings->setMinAmount(sEditMinAmount->value());
}

void CommonSettingsForOffersWidget::cancel()
{
    setData();
}

void CommonSettingsForOffersWidget::setData()
{
    cBoxCountry->setCurrentData(settings->getCountryIso(CommonSettingsForOffers::EditOffer));
    cBoxCurrency->setCurrentData(settings->getCurrencyIso(CommonSettingsForOffers::EditOffer));
    cBoxPayment->setCurrentData(QString::number(settings->getPaymentMethodType(CommonSettingsForOffers::EditOffer)));
    sEditMinAmount->setValue(settings->getMinAmount());
}

void CommonSettingsForOffersWidget::changedData(int)
{
    Q_EMIT dataChanged();
}

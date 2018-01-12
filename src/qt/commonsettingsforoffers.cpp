#include "commonsettingsforoffers.h"
#include "util.h"

CommonSettingsForOffers *CommonSettingsForOffers::pSingleton = nullptr;
int CommonSettingsForOffers::nCounter = 0;

CommonSettingsForOffers::CommonSettingsForOffers()
{
    boost::filesystem::path pathSettings = GetDataDir() / "dex.conf";
    QString strPathSetting = QString::fromStdWString(pathSettings.wstring());

    settings = new QSettings(strPathSetting, QSettings::IniFormat);
}

CommonSettingsForOffers::~CommonSettingsForOffers()
{
    delete settings;
}

CommonSettingsForOffers *CommonSettingsForOffers::instance()
{
    if (pSingleton == nullptr) {
        pSingleton = new CommonSettingsForOffers();
    }

    nCounter++;

    return pSingleton;
}

void CommonSettingsForOffers::freeInstance()
{
    if (nCounter > 0) {
        nCounter--;

        if (nCounter == 0) {
            delete pSingleton;
            pSingleton = nullptr;
        }
    }
}

QString CommonSettingsForOffers::getCountryIso()
{
    settings->beginGroup("default");
    QString iso = settings->value("countryIso", "RU").toString();
    settings->endGroup();

    return iso;
}

void CommonSettingsForOffers::setCountryIso(const QString &iso)
{
    settings->beginGroup("default");
    settings->setValue("countryIso", iso);
    settings->endGroup();
}

QString CommonSettingsForOffers::getCurrencyIso()
{
    settings->beginGroup("default");
    QString iso = settings->value("currencyIso", "RUB").toString();
    settings->endGroup();

    return iso;
}

void CommonSettingsForOffers::setCurrencyIso(const QString &iso)
{
    settings->beginGroup("default");
    settings->setValue("currencyIso", iso);
    settings->endGroup();
}

quint8 CommonSettingsForOffers::getPaymentMethodType()
{
    settings->beginGroup("default");
    quint8 type = settings->value("paymentMethodType", 1).toReal();
    settings->endGroup();

    return type;
}

void CommonSettingsForOffers::setPaymentMethodType(const quint8 &type)
{
    settings->beginGroup("default");
    settings->setValue("paymentMethodType", type);
    settings->endGroup();
}

quint64 CommonSettingsForOffers::getMinAmount()
{
    settings->beginGroup("default");
    quint64 min = settings->value("minAmount", 5).toReal();
    settings->endGroup();

    return min;
}

void CommonSettingsForOffers::setMinAmount(const quint64 &min)
{
    settings->beginGroup("default");
    settings->setValue("minAmount", min);
    settings->endGroup();
}

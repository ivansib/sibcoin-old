#include "commonsettingsforoffers.h"
#include "util.h"
#include "dex/db/dexdto.h"

CommonSettingsForOffers *CommonSettingsForOffers::pSingleton = nullptr;
int CommonSettingsForOffers::nCounter = 0;

CommonSettingsForOffers::CommonSettingsForOffers()
{
    boost::filesystem::path pathSettings = GetDataDir() / dex::DEX_CONFIG;
    QString strPathSetting = QString::fromStdWString(pathSettings.wstring());

    settings = new QSettings(strPathSetting, QSettings::IniFormat);
}

CommonSettingsForOffers::~CommonSettingsForOffers()
{
    delete settings;
}

void CommonSettingsForOffers::sync()
{
    settings->sync();
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

QString CommonSettingsForOffers::getCountryIso(const TypeSettings &t)
{
    QString def = "all";

    if (t == TypeSettings::EditOffer) {
        def = "RU";
    }

    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    QString iso = settings->value("countryIso", def).toString();
    settings->endGroup();

    return iso;
}

void CommonSettingsForOffers::setCountryIso(const TypeSettings &t, const QString &iso)
{
    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    settings->setValue("countryIso", iso);
    settings->endGroup();
}

QString CommonSettingsForOffers::getCurrencyIso(const TypeSettings &t)
{
    QString def = "all";

    if (t == TypeSettings::EditOffer) {
        def = "RUB";
    }

    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    QString iso = settings->value("currencyIso", def).toString();
    settings->endGroup();

    return iso;
}

void CommonSettingsForOffers::setCurrencyIso(const TypeSettings &t, const QString &iso)
{
    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    settings->setValue("currencyIso", iso);
    settings->endGroup();
}

quint8 CommonSettingsForOffers::getPaymentMethodType(const TypeSettings &t)
{
    int def = 0;

    if (t == TypeSettings::EditOffer) {
        def = 128;
    }

    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    quint8 type = settings->value("paymentMethodType", def).toReal();
    settings->endGroup();

    return type;
}

void CommonSettingsForOffers::setPaymentMethodType(const TypeSettings &t, const quint8 &type)
{
    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    settings->setValue("paymentMethodType", type);
    settings->endGroup();
}

int CommonSettingsForOffers::getTypeOffer(const CommonSettingsForOffers::TypeSettings &t)
{
    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    int type = settings->value("typeOffer", 0).toInt();
    settings->endGroup();

    return type;
}

void CommonSettingsForOffers::setTypeOffer(const CommonSettingsForOffers::TypeSettings &t, const int &type)
{
    QString group = QString("default/%1").arg(QString::number(t));
    settings->beginGroup(group);
    settings->setValue("typeOffer", type);
    settings->endGroup();
}

quint64 CommonSettingsForOffers::getMinAmount()
{
    settings->beginGroup("default");
    quint64 min = settings->value("minAmount", 500000000).toReal();
    settings->endGroup();

    return min;
}

void CommonSettingsForOffers::setMinAmount(const quint64 &min)
{
    settings->beginGroup("default");
    settings->setValue("minAmount", min);
    settings->endGroup();
}

int CommonSettingsForOffers::getNumRowsTables()
{
    settings->beginGroup("default");
    int num = settings->value("numRowsTables", dex::DEX_MAX_ROWS_OUTPUT).toInt();
    settings->endGroup();

    return num;
}

void CommonSettingsForOffers::setNumRowsTables(const int &num)
{
    settings->beginGroup("default");
    settings->setValue("numRowsTables", num);
    settings->endGroup();
}

bool CommonSettingsForOffers::getShowMaxRowsTables()
{
    settings->beginGroup("default");
    bool isShow = settings->value("showMaxRowsTables", false).toBool();
    settings->endGroup();

    return isShow;
}

void CommonSettingsForOffers::setShowMaxRowsTables(const bool isShow)
{
    settings->beginGroup("default");
    settings->setValue("showMaxRowsTables", isShow);
    settings->endGroup();
}

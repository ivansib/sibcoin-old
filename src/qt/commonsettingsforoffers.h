#ifndef COMMONSETTINGSFOROFFERS_H
#define COMMONSETTINGSFOROFFERS_H

#include <QSettings>

class CommonSettingsForOffers
{
    CommonSettingsForOffers();
    CommonSettingsForOffers(const CommonSettingsForOffers &) {}
    CommonSettingsForOffers &operator=(const CommonSettingsForOffers &) {return *this;}
    ~CommonSettingsForOffers();

    static CommonSettingsForOffers *pSingleton;
    static int nCounter;

public:
    static CommonSettingsForOffers *instance();
    static void freeInstance();

    QString getCountryIso();
    void setCountryIso(const QString &iso);

    QString getCurrencyIso();
    void setCurrencyIso(const QString &iso);

    quint8 getPaymentMethodType();
    void setPaymentMethodType(const quint8 &type);

    quint64 getMinAmount();
    void setMinAmount(const quint64 &min);

private:
    QSettings *settings;
};

#endif

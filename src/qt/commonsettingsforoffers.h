#ifndef COMMONSETTINGSFOROFFERS_H
#define COMMONSETTINGSFOROFFERS_H

#include <QSettings>

class CommonSettingsForOffers
{

public:
    CommonSettingsForOffers();
    ~CommonSettingsForOffers();

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

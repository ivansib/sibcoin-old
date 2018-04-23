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
    enum TypeSettings {
        BuyOffer,
        SellOffer,
        MyOffer,
        EditOffer
    };

    static CommonSettingsForOffers *instance();
    static void freeInstance();

    QString getCountryIso(const TypeSettings &t);
    void setCountryIso(const TypeSettings &t, const QString &iso);

    QString getCurrencyIso(const TypeSettings &t);
    void setCurrencyIso(const TypeSettings &t, const QString &iso);

    quint8 getPaymentMethodType(const TypeSettings &t);
    void setPaymentMethodType(const TypeSettings &t, const quint8 &type);

    int getTypeOffer(const TypeSettings &t);
    void setTypeOffer(const TypeSettings &t, const int &type);

    quint64 getMinAmount();
    void setMinAmount(const quint64 &min);

    int getNumRowsTables();
    void setNumRowsTables(const int &num);

    bool getShowMaxRowsTables();
    void setShowMaxRowsTables(const bool isShow);

private:
    QSettings *settings;
};

#endif

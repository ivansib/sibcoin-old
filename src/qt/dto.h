#ifndef DTO_H
#define DTO_H

#include <QString>
#include "dex/dexdto.h"

using namespace dex;

struct QtCountryInfo {
    QString iso;
    QString name;
    bool enabled;
};

struct QtCurrencyInfo {
    QString iso;
    QString name;
    QString symbol;
    bool enabled;
};

struct QtOfferInfo {
    QString idTransaction;
    QString hash;
    QString countryIso;
    QString currencyIso;
    quint8 paymentMethod;
    quint64 price;
    quint64 minAmount;
    quint64 timeCreate;
    int timeToExpiration;
    QString shortInfo;
    QString details;
};

struct QtMyOfferInfo : QtOfferInfo {
    TypeOffer type;
    StatusOffer status;

    QtOfferInfo getOfferInfo() const {
        QtOfferInfo info;

        info.idTransaction = idTransaction;
        info.hash = hash;
        info.countryIso = countryIso;
        info.currencyIso = currencyIso;
        info.paymentMethod = paymentMethod;
        info.price = price;
        info.minAmount = minAmount;
        info.timeCreate = timeCreate;
        info.timeToExpiration = timeToExpiration;
        info.shortInfo = shortInfo;
        info.details = details;

        return info;
    }

    void setOfferInfo(const QtOfferInfo &info) {
        idTransaction = info.idTransaction;
        hash = info.hash;
        countryIso = info.countryIso;
        currencyIso = info.currencyIso;
        paymentMethod = info.paymentMethod;
        price = info.price;
        minAmount = info.minAmount;
        timeCreate = info.timeCreate;
        timeToExpiration = info.timeToExpiration;
        shortInfo = info.shortInfo;
        details = info.details;
    }
};

#endif

#ifndef DTO_H
#define DTO_H

#include <QString>
#include "dex/db/dexdto.h"

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
    QString pubKey;
    QString countryIso;
    QString currencyIso;
    quint8 paymentMethod;
    quint64 price;
    quint64 minAmount;
    quint64 timeCreate;
    quint64 timeToExpiration;
    quint64 timeModification;
    QString shortInfo;
    QString details;
    int editingVersion;
};

struct QtMyOfferInfo : QtOfferInfo {
    TypeOffer type;
    StatusOffer status;

    QtOfferInfo getOfferInfo() const {
        QtOfferInfo info;

        info.idTransaction = idTransaction;
        info.hash = hash;
        info.pubKey = pubKey;
        info.countryIso = countryIso;
        info.currencyIso = currencyIso;
        info.paymentMethod = paymentMethod;
        info.price = price;
        info.minAmount = minAmount;
        info.timeCreate = timeCreate;
        info.timeToExpiration = timeToExpiration;
        info.timeModification = timeModification;
        info.shortInfo = shortInfo;
        info.details = details;
        info.editingVersion = editingVersion;

        return info;
    }

    void setOfferInfo(const QtOfferInfo &info) {
        idTransaction = info.idTransaction;
        hash = info.hash;
        pubKey = info.pubKey;
        countryIso = info.countryIso;
        currencyIso = info.currencyIso;
        paymentMethod = info.paymentMethod;
        price = info.price;
        minAmount = info.minAmount;
        timeCreate = info.timeCreate;
        timeToExpiration = info.timeToExpiration;
        timeModification = info.timeModification;
        shortInfo = info.shortInfo;
        details = info.details;
        editingVersion = info.editingVersion;
    }
};

#endif

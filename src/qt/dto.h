#ifndef DTO_H
#define DTO_H

#include <QString>

struct QtOfferInfo {
    QString idTransaction;
    QString hash;
    QString countryIso;
    QString currencyIso;
    quint8 paymentMethod;
    quint64 price;
    quint64 minAmount;
    quint64 timeCreate;
    quint64 timeExpiration;
    QString shortInfo;
    QString details;
};

#endif

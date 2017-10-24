#ifndef CONVERTDATA_H
#define CONVERTDATA_H

#include <QList>
#include "dex/dexdto.h"
#include "dto.h"

using namespace dex;

const int decimals = 8;

class ConvertData {
public:
    static QtOfferInfo toQtOfferInfo(const OfferInfo &offer);
    static QList<QtOfferInfo> toListQtOfferInfo(const std::list<OfferInfo> &offers);

    static OfferInfo fromQtOfferInfo(const QtOfferInfo &qtOffer);

    static QString toQString(const std::string &str);
    static QString toTr(const std::string &str);
    static std::string fromQString(const QString &str);

    static QString toUiPrice(const quint64 &i);
    static quint64 fromUiPrice(QString str);
};

#endif

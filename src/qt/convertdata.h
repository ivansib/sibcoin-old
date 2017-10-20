#ifndef CONVERTDATA_H
#define CONVERTDATA_H

#include <QList>
#include "dex/dexdto.h"
#include "dto.h"

using namespace dex;

class ConvertData {
public:
    static QtOfferInfo toQtOfferInfo(const OfferInfo &offer);
    static QList<QtOfferInfo> toListQtOfferInfo(const std::list<OfferInfo> &offers);

    static OfferInfo fromQtOfferInfo(const QtOfferInfo &qtOffer);

    static QString toQString(const std::string &str);
    static QString toTr(const std::string &str);
    static std::string fromQString(const QString &str);
};

#endif

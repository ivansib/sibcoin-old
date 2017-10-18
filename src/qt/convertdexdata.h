#ifndef CONVERTDEXDATA_H
#define CONVERTDEXDATA_H

#include <QList>
#include "dex/dexdto.h"
#include "dto.h"

using namespace dex;

class ConvertDexData {
public:
    static QtOfferInfo toQtOfferInfo(const OfferInfo &offer);
    static QList<QtOfferInfo> toListQtOfferInfo(const std::list<OfferInfo> &offers);
};

#endif

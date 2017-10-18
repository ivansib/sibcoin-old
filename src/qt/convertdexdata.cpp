#include "convertdexdata.h"

QtOfferInfo ConvertDexData::toQtOfferInfo(const OfferInfo &offer)
{
    QtOfferInfo qtOffer;

    qtOffer.idTransaction = QString::fromUtf8(offer.idTransaction.GetHex().c_str());
    qtOffer.hash = QString::fromUtf8(offer.hash.GetHex().c_str());
    qtOffer.countryIso = QString::fromUtf8(offer.countryIso.c_str());
    qtOffer.currencyIso = QString::fromUtf8(offer.currencyIso.c_str());
    qtOffer.paymentMethod = offer.paymentMethod;
    qtOffer.price = offer.price;
    qtOffer.minAmount = offer.minAmount;
    qtOffer.timeCreate = offer.timeCreate;
    qtOffer.timeExpiration = offer.timeExpiration;
    qtOffer.shortInfo = QString::fromUtf8(offer.shortInfo.c_str());
    qtOffer.details = QString::fromUtf8(offer.details.c_str());

    return qtOffer;
}

QList<QtOfferInfo> ConvertDexData::toListQtOfferInfo(const std::list<OfferInfo> &offers)
{
    QList<QtOfferInfo> qtOffers;

    for (auto item : offers) {
        qtOffers << ConvertDexData::toQtOfferInfo(item);
    }

    return qtOffers;
}

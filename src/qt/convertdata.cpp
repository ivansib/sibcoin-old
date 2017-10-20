#include <QObject>
#include "convertdata.h"

QtOfferInfo ConvertData::toQtOfferInfo(const OfferInfo &offer)
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
    qtOffer.timeToExpiration = offer.timeToExpiration;
    qtOffer.shortInfo = QString::fromUtf8(offer.shortInfo.c_str());
    qtOffer.details = QString::fromUtf8(offer.details.c_str());

    return qtOffer;
}

QList<QtOfferInfo> ConvertData::toListQtOfferInfo(const std::list<OfferInfo> &offers)
{
    QList<QtOfferInfo> qtOffers;

    for (auto item : offers) {
        qtOffers << ConvertData::toQtOfferInfo(item);
    }

    return qtOffers;
}

OfferInfo ConvertData::fromQtOfferInfo(const QtOfferInfo &qtOffer)
{
    OfferInfo offer;

    offer.idTransaction.SetHex(qtOffer.idTransaction.toUtf8().constData());
    offer.hash.SetHex(qtOffer.hash.toUtf8().constData());
    offer.countryIso = qtOffer.countryIso.toUtf8().constData();
    offer.currencyIso = qtOffer.currencyIso.toUtf8().constData();
    offer.paymentMethod = qtOffer.paymentMethod;
    offer.price = qtOffer.price;
    offer.minAmount = qtOffer.minAmount;
    offer.timeCreate = qtOffer.timeCreate;
    offer.timeToExpiration = qtOffer.timeToExpiration;
    offer.shortInfo = qtOffer.shortInfo.toUtf8().constData();
    offer.details = qtOffer.details.toUtf8().constData();

    return offer;
}

QString ConvertData::toQString(const std::string &str)
{
    return QString::fromUtf8(str.c_str());
}

std::string ConvertData::fromQString(const QString &str)
{
    return str.toUtf8().constData();
}

QString ConvertData::toTr(const std::string &str)
{
    return QObject::tr(str.c_str());
}

#include <QObject>
#include <QStringList>
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

QtMyOfferInfo ConvertData::toQtMyOfferInfo(const MyOfferInfo &offer)
{
    QtMyOfferInfo qtOffer;

    qtOffer.type = offer.type;
    qtOffer.status = offer.status;

    qtOffer.setOfferInfo(ConvertData::toQtOfferInfo(offer.getOfferInfo()));

    return qtOffer;
}

QList<QtMyOfferInfo> ConvertData::toListQtMyOfferInfo(const std::list<MyOfferInfo> &offers)
{
    QList<QtMyOfferInfo> qtOffers;

    for (auto item : offers) {
        qtOffers << ConvertData::toQtMyOfferInfo(item);
    }

    return qtOffers;
}

MyOfferInfo ConvertData::fromQtMyOfferInfo(const QtMyOfferInfo &qtOffer)
{
    MyOfferInfo offer;

    offer.type = qtOffer.type;
    offer.status = qtOffer.status;

    offer.setOfferInfo(ConvertData::fromQtOfferInfo(qtOffer.getOfferInfo()));

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

QString ConvertData::toUiPrice(const quint64 &i)
{
    QString num = QString::number(i);
    if (num.size() >= decimals) {
        int dot = num.size()-decimals;

        num = num.insert(dot, '.');

        bool stop = false;
        while (!stop) {
            int size = num.size();
            if (size > dot && num[size-1] == '0') {
                num = num.mid(0, size-1);
            } else {
                stop = true;
            }
        }

        if (num[num.size() - 1] == '.') {
            num = num.mid(0, num.size()-1);
        }
    } else {
        num = "0";
    }

    return num;
}

quint64 ConvertData::fromUiPrice(QString str)
{
    if (str.indexOf('.') > -1) {
        QStringList l = str.split('.');
        QString dec = l[1];
        if (dec.size() < decimals) {
            int size = decimals-dec.size();
            for (int i = 0; i < size; i++) {
                dec += "0";
            }
        }

        str = l[0] + dec;
    } else {
        QString zero("0");
        zero = zero.repeated(decimals);
        str += zero;
    }
    quint64 num = str.toULongLong();
    return num;
}

QString ConvertData::toTr(const std::string &str)
{
    return QObject::tr(str.c_str());
}

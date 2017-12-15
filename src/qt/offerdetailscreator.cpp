#include "offerdetailscreator.h"

OfferDetailsCreator::OfferDetailsCreator(DexDB *db, QDialog *parent) : OfferDetails(db, OfferDetails::Create, parent)
{
}

void OfferDetailsCreator::initData()
{
    sBoxExpiration->setValue(10);
}

QtMyOfferInfo OfferDetailsCreator::getMyOffer() const
{
    QtMyOfferInfo info;
    info.type = static_cast<TypeOffer>(cBoxOffer->currentIndex());
    info.countryIso = cBoxCountry->currentData().toString();
    info.currencyIso = cBoxCurrency->currentData().toString();
    info.paymentMethod = cBoxPayment->currentData().toInt();
    info.price = sBoxPrice->value();
    info.minAmount = sBoxMinAmount->value();
    info.timeCreate = QDateTime::currentDateTime().toTime_t();
    info.timeToExpiration = sBoxExpiration->value();
    info.shortInfo = tEditShortInfo->toPlainText();
    info.details = tEditDetails->toPlainText();

    return info;
}

void OfferDetailsCreator::saveData()
{
    QtMyOfferInfo info = getMyOffer();

    Q_EMIT dataSave(info);
    close();
}

void OfferDetailsCreator::sendData()
{
    QtMyOfferInfo info = getMyOffer();

    Q_EMIT dataSend(info);
    close();
}

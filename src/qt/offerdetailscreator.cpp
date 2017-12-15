#include "offerdetailscreator.h"

OfferDetailsCreator::OfferDetailsCreator(DexDB *db, QDialog *parent) : OfferDetails(db, OfferDetails::Create, parent)
{
}

void OfferDetailsCreator::initData()
{
    cBoxOffer->setCurrentIndex(0);
    cBoxCountry->setCurrentIndex(0);
    cBoxCurrency->setCurrentIndex(0);
    cBoxPayment->setCurrentIndex(0);
    sBoxPrice->setValue(0);
    sBoxMinAmount->setValue(0);
    sBoxExpiration->setValue(10);
    tEditShortInfo->setText("");
    tEditDetails->setText("");
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

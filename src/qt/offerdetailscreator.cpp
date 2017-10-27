#include "offerdetailscreator.h"

OfferDetailsCreator::OfferDetailsCreator(DexDB *db, QDialog *parent) : OfferDetails(db, OfferDetails::Create, parent)
{
}

void OfferDetailsCreator::initData()
{
    sBoxExpiration->setValue(10);
}

void OfferDetailsCreator::saveData()
{
    QtMyOfferInfo info;
//    info.idTransaction = lIdView->text();
//    info.hash = lHashView->text();
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

    Q_EMIT dataChanged(info);
    close();
}

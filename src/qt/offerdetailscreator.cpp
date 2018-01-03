#include "offerdetailscreator.h"
#include "random.h"

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
    cBoxExpiration->setCurrentIndex(0);
    tEditShortInfo->setText("");
    tEditDetails->setText("");

    changedTimeToExpiration(0);
}

void OfferDetailsCreator::setModel(WalletModel *model)
{
    this->model = model;
}

QtMyOfferInfo OfferDetailsCreator::getMyOffer() const
{
    QtMyOfferInfo info;

    CKey secret = model->generateNewKey();
    CPubKey pubkey = secret.GetPubKey();
    info.pubKey = QString::fromUtf8(pubkey.GetHash().GetHex().c_str());

    info.type = static_cast<TypeOffer>(cBoxOffer->currentIndex());
    info.countryIso = cBoxCountry->currentData().toString();
    info.currencyIso = cBoxCurrency->currentData().toString();
    info.paymentMethod = cBoxPayment->currentData().toInt();
    info.price = sBoxPrice->value();
    info.minAmount = sBoxMinAmount->value();
    info.timeCreate = QDateTime::currentDateTime().toTime_t();
    info.timeToExpiration = cBoxExpiration->currentText().toInt();
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

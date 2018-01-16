#include "offerdetailscreator.h"
#include "random.h"

OfferDetailsCreator::OfferDetailsCreator(DexDB *db, QDialog *parent) : OfferDetails(db, parent)
{
    setupUi(this);

    for (auto item : expirations) {
        cBoxExpiration->addItem(QString::number(item));
    }

    addBtnSend(btnSend);
    addBtnSaveDraft(btnSaveDraft);
    addBtnCancel(btnCancel);

    addCBoxOffer(cBoxOffer);
    addCBoxPayment(cBoxPayment);
    addCBoxCountry(cBoxCountry);
    addCBoxCurrency(cBoxCurrency);

    addTEditShortInfo(tEditShortInfo);

    addExpiration(cBoxExpiration, lEditTimeExpiration);

    addLEditTransactionPrice(lEditTransactionPrice);

    updateNavigationData();
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

    lEditTimeCreate->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
    changedTimeToExpiration(0);
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
    info.timeToExpiration = QDateTime::currentDateTime().addDays(cBoxExpiration->currentText().toInt()).toTime_t();;
    info.shortInfo = tEditShortInfo->toPlainText();
    info.details = tEditDetails->toPlainText();
    info.status = Indefined;
    info.editingVersion = 0;

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

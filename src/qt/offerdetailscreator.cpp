#include <QMessageBox>
#include "offerdetailscreator.h"
#include "random.h"

OfferDetailsCreator::OfferDetailsCreator(DexDB *db, QDialog *parent) : OfferDetails(db, parent)
{
    setupUi(this);

    settings = CommonSettingsForOffers::instance();

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

OfferDetailsCreator::~OfferDetailsCreator()
{
    settings->freeInstance();
}

void OfferDetailsCreator::initData()
{
    cBoxOffer->setCurrentIndex(0);
    cBoxCountry->setCurrentData(settings->getCountryIso());
    cBoxCurrency->setCurrentData(settings->getCurrencyIso());
    cBoxPayment->setCurrentData(QString::number(settings->getPaymentMethodType()));
    sBoxPrice->setValue(0);
    sBoxMinAmount->setValue(settings->getMinAmount());
    cBoxExpiration->setCurrentIndex(0);
    tEditShortInfo->setText("");
    tEditDetails->setText("");

    lEditTimeCreate->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
    changedTimeToExpiration(0);
}

QtMyOfferInfo OfferDetailsCreator::getMyOffer(bool &isError) const
{
    isError = false;
    QtMyOfferInfo info;

    CKey secret = model->generateNewKey();
    CPubKey pubkey = secret.GetPubKey();
    if (!model->addKeyPubKey(secret, pubkey)) {
        QMessageBox::critical(0, QObject::tr("Error"), tr("Error: Can't add public key to the wallet"));
        isError = true;
        return info;
    }

    info.pubKey = QString::fromUtf8(HexStr(pubkey.begin(), pubkey.end()).c_str());

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
    bool isError;
    QtMyOfferInfo info = getMyOffer(isError);

    if (!isError) {
        Q_EMIT dataSave(info);
        close();
    }
}

void OfferDetailsCreator::sendData()
{
    if (confirmationSend()) {
        bool isError;
        QtMyOfferInfo info = getMyOffer(isError);

        if (!isError) {
            Q_EMIT dataSend(info);
            close();
        }
    }
}

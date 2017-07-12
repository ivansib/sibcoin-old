#include <QMessageBox>
#include "offerdetailseditor.h"
#include "dex/dexsync.h"

OfferDetailsEditor::OfferDetailsEditor(DexDB *db, QDialog *parent) : OfferDetails(db, parent)
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

    connect(btnDeleteDraft, &QPushButton::clicked, this, &OfferDetailsEditor::deleteDraftData);

    updateNavigationData();
}

void OfferDetailsEditor::setOfferInfo(const QtMyOfferInfo &info)
{
    btnDeleteDraft->setVisible(true);
    btnSaveDraft->setVisible(true);
    btnSend->setVisible(true);

    if (!dexsync.isSynced()) {
        btnSend->setEnabled(false);
    } else {
        btnSend->setEnabled(true);
    }

    offerInfo = info;

    lPubKeyView->setText(info.pubKey);
    lIdView->setText(info.idTransaction);
    lHashView->setText(info.hash);
    lStatusView->setText(status(info.status));
    cBoxOffer->setCurrentText(offerType(info.type));
    cBoxCountry->setCurrentData(info.countryIso);
    cBoxCurrency->setCurrentData(info.currencyIso);
    cBoxPayment->setCurrentData(QString::number(info.paymentMethod));
    sBoxPrice->setValue(info.price);
    sBoxMinAmount->setValue(info.minAmount);

    if (info.status == Active) {
        isApproximateExpiration(false);
        enabledHashEditLines(false);

        btnDeleteDraft->setVisible(false);
        btnSaveDraft->setVisible(false);

        lEditTimeCreate->setText(QDateTime::fromTime_t(info.timeCreate).toString("dd.MM.yyyy hh:mm"));
        lEditTimeExpiration->setText(QDateTime::fromTime_t(info.timeToExpiration).toString("dd.MM.yyyy hh:mm"));

        int leftEdits = maxNumEditsMyOffer - info.editingVersion;

        lEditLeftEdits->setText(QString::number(leftEdits));

        if (leftEdits <= 0) {
            btnSend->setVisible(false);
        }
    } else {
        isApproximateExpiration(true);
        enabledHashEditLines(true);

        lEditTimeCreate->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
        lEditTimeExpiration->setText(QDateTime::fromTime_t(info.timeToExpiration).toString("dd.MM.yyyy hh:mm"));
    }

    turnLines(info.status);

    int days = ((info.timeToExpiration - info.timeCreate - 1) / 86400) +1;
    days = (((days -1)/10)+1)*10;
    int index = expirations.indexOf(days);

    cBoxExpiration->setCurrentIndex(index);

    changedTimeToExpiration(index);

    tEditShortInfo->setText(info.shortInfo);
    tEditDetails->setText(info.details);
}

QString OfferDetailsEditor::status(const StatusOffer &s) const
{
    QString str;

    switch (s) {
    case Active:
        str = tr("Active");
        break;
    case Draft:
        str = tr("Draft");
        break;
    case Expired:
        str = tr("Expired");
        break;
    case Cancelled:
        str = tr("Cancelled");
        break;
    case Suspended:
        str = tr("Suspended");
        break;
    case Unconfirmed:
        str = tr("Unconfirmed");
        break;
    default:
        break;
    }

    return str;
}

QString OfferDetailsEditor::offerType(const TypeOffer &s) const
{
    QString str;

    switch (s) {
    case Buy:
        str = tr("Buy");
        break;
    case Sell:
        str = tr("Sell");
        break;
    default:
        break;
    }

    return str;
}

void OfferDetailsEditor::enabledHashEditLines(const bool &b)
{
    cBoxOffer->setEnabled(b);
    cBoxCountry->setEnabled(b);
    cBoxCurrency->setEnabled(b);
    cBoxPayment->setEnabled(b);
    sBoxMinAmount->setEnabled(b);
    cBoxExpiration->setEnabled(b);
}

void OfferDetailsEditor::turnLines(const StatusOffer &status)
{
    if (status == Active) {
        lTransactionPrice->setVisible(false);
        lEditTransactionPrice->setVisible(false);
        lLeftEdits->setVisible(true);
        lEditLeftEdits->setVisible(true);
    } else {
        lTransactionPrice->setVisible(true);
        lEditTransactionPrice->setVisible(true);
        lLeftEdits->setVisible(false);
        lEditLeftEdits->setVisible(false);
    }
}

void OfferDetailsEditor::updateMyOffer()
{
    if (offerInfo.status == Active) {
        offerInfo.price = sBoxPrice->value();
        offerInfo.shortInfo = tEditShortInfo->toPlainText();
        offerInfo.details = tEditDetails->toPlainText();
    } else {
        offerInfo.countryIso = cBoxCountry->currentData().toString();
        offerInfo.currencyIso = cBoxCurrency->currentData().toString();
        offerInfo.paymentMethod = cBoxPayment->currentData().toInt();
        offerInfo.price = sBoxPrice->value();
        offerInfo.minAmount = sBoxMinAmount->value();
        offerInfo.timeCreate = QDateTime::currentDateTime().toTime_t();
        offerInfo.timeToExpiration = QDateTime::currentDateTime().addDays(cBoxExpiration->currentText().toInt()).toTime_t();
        offerInfo.shortInfo = tEditShortInfo->toPlainText();
        offerInfo.details = tEditDetails->toPlainText();
    }
}

void OfferDetailsEditor::isApproximateExpiration(const bool &b)
{
    if (b) {
        lInfoExpiration->setText(tr("Approximate Time Expiration:"));
    } else {
        lInfoExpiration->setText(tr("Time Expiration:"));
    }
}

void OfferDetailsEditor::saveData()
{
    updateMyOffer();

    Q_EMIT dataSave(offerInfo);
    close();
}

void OfferDetailsEditor::sendData()
{
    if (confirmationSend()) {
        updateMyOffer();

        Q_EMIT dataSend(offerInfo);
        close();
    }
}

void OfferDetailsEditor::deleteDraftData()
{
    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Delete draft"),
        tr("Confirm delete draft?"),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if(retval == QMessageBox::Yes) {
        Q_EMIT draftDataDelete(offerInfo);
        close();
    }
}
#include <QDateTime>
#include "offerdetailseditor.h"

OfferDetailsEditor::OfferDetailsEditor(DexDB *db, QDialog *parent) : OfferDetails(db, OfferDetails::Edit, parent)
{

}

void OfferDetailsEditor::setOfferInfo(const QtMyOfferInfo &info)
{
    QDateTime timeExpiration = QDateTime::fromTime_t(info.timeCreate).addDays(info.timeToExpiration);

    lEditId->setText(info.idTransaction);
    lEditHash->setText(info.hash);
    cBoxCountry->setCurrentData(info.countryIso);
    cBoxCurrency->setCurrentData(info.currencyIso);
    cBoxPayment->setCurrentData(QString::number(info.paymentMethod));
    sBoxPrice->setValue(info.price);
    sBoxMinAmount->setValue(info.minAmount);
    lEditTimeCreate->setText(QDateTime::fromTime_t(info.timeCreate).toString("dd.MM.yyyy hh:mm"));
    sBoxExpiration->setValue(info.timeToExpiration);
    lEditTimeExpiration->setText(timeExpiration.toString("dd.MM.yyyy hh:mm"));
    tEditShortInfo->setText(info.shortInfo);
    tEditDetails->setText(info.details);
}

void OfferDetailsEditor::saveData()
{
    QtMyOfferInfo info;
    info.idTransaction = lEditId->text();
    info.hash = lEditHash->text();
    info.countryIso = cBoxCountry->currentData().toString();
    info.currencyIso = cBoxCurrency->currentData().toString();
    info.paymentMethod = cBoxPayment->currentData().toInt();
    info.price = sBoxPrice->value();
    info.minAmount = sBoxMinAmount->value();
    info.timeCreate = QDateTime::fromString(lEditTimeCreate->text(), "dd.MM.yyyy hh:mm").toTime_t();
    info.timeToExpiration = sBoxExpiration->value();
    info.shortInfo = tEditShortInfo->toPlainText();
    info.details = tEditDetails->toPlainText();

    Q_EMIT dataChanged(info);
    close();
}

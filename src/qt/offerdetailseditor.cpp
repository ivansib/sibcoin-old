#include "offerdetailseditor.h"

OfferDetailsEditor::OfferDetailsEditor(DexDB *db, QDialog *parent) : OfferDetails(db, OfferDetails::Edit, parent)
{

}

void OfferDetailsEditor::setOfferInfo(const QtMyOfferInfo &info)
{
    QDateTime timeExpiration = QDateTime::fromTime_t(info.timeCreate).addDays(info.timeToExpiration);

    lIdView->setText(info.idTransaction);
    lHashView->setText(info.hash);
    lStatusView->setText(status(info.status));
    lOfferInfoView->setText(offerType(info.type));
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

void OfferDetailsEditor::saveData()
{
    QtMyOfferInfo info;
    info.idTransaction = lIdView->text();
    info.hash = lHashView->text();
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

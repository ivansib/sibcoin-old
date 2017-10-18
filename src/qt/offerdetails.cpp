#include <QDateTime>
#include "offerdetails.h"
#include "ui_offerdetails.h"

OfferDetails::OfferDetails(QDialog *parent) : QDialog(parent), ui(new Ui::OfferDetails)
{
    ui->setupUi(this);
}

OfferDetails::~OfferDetails()
{
    delete ui;
}

void OfferDetails::setOfferInfo(const QtOfferInfo &info, const QString &country, const QString &currency, const QString &payment)
{
    ui->lEditId->setText(info.idTransaction);
    ui->lEditHash->setText(info.hash);
    ui->lEditCountry->setText(country);
    ui->lEditCurrency->setText(currency);
    ui->lEditPayment->setText(payment);
    ui->lEditPrice->setText(QString::number(info.price));
    ui->lEditMinAmount->setText(QString::number(info.minAmount));
    ui->lEditTimeCreate->setText(QDateTime::fromTime_t(info.timeCreate).toString("dd.MM.yyyy hh:mm"));
    ui->lEditTimeExpiration->setText(QDateTime::fromTime_t(info.timeExpiration).toString("dd.MM.yyyy hh:mm"));
    ui->tEditShortInfo->setText(info.shortInfo);
    ui->tEditDetails->setText(info.details);
}

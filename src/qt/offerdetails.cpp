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

void OfferDetails::setOfferInfo(const OfferInfo &info)
{
    this->info = info;

    ui->lEditId->setText(QString::fromUtf8(info.idTransaction.GetHex().c_str()));
    ui->lEditHash->setText(QString::fromUtf8(info.hash.GetHex().c_str()));
    ui->lEditCountry->setText(QString::fromUtf8(info.countryIso.c_str()));
    ui->lEditCurrency->setText(QString::fromUtf8(info.currencyIso.c_str()));
    ui->lEditPayment->setText(QString::number(info.paymentMethod));
    ui->lEditPrice->setText(QString::number(info.price));
    ui->lEditMinAmount->setText(QString::number(info.minAmount));
    ui->lEditTimeCreate->setText(QString::number(info.timeCreate));
    ui->lEditTimeExpiration->setText(QString::number(info.timeExpiration));
    ui->tEditShortInfo->setText(QString::fromUtf8(info.shortInfo.c_str()));
    ui->tEditDetails->setText(QString::fromUtf8(info.details.c_str()));
}

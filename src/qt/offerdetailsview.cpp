#include <QDateTime>
#include "offerdetailsview.h"
#include "convertdata.h"
#include "ui_offerdetailsview.h"

OfferDetailsView::OfferDetailsView(DexDB *db, QDialog *parent) : QDialog(parent), ui(new Ui::OfferDetailsView), db(db)
{
    ui->setupUi(this);

    connect(ui->btnClose, &QPushButton::clicked, this, &OfferDetailsView::close);
}

OfferDetailsView::~OfferDetailsView()
{
    delete ui;
}

void OfferDetailsView::setOfferInfo(const QtOfferInfo &info)
{
    QDateTime timeExpiration = QDateTime::fromTime_t(info.timeCreate).addDays(info.timeToExpiration);

    ui->lEditPubKey->setText(info.pubKey);
    ui->lEditId->setText(info.idTransaction);
    ui->lEditHash->setText(info.hash);

    auto countryInfo = db->getCountryInfo(ConvertData::fromQString(info.countryIso));
    QString country = ConvertData::toTr(countryInfo.name);
    ui->lEditCountry->setText(country);

    auto currencyInfo = db->getCurrencyInfo(ConvertData::fromQString(info.currencyIso));
    QString currency = ConvertData::toTr(currencyInfo.name);
    ui->lEditCurrency->setText(currency);

    auto paymentInfo = db->getPaymentMethodInfo((unsigned char)info.paymentMethod);
    QString payment = ConvertData::toTr(paymentInfo.name);
    ui->lEditPayment->setText(payment);

    ui->lEditPrice->setText(ConvertData::toUiPrice(info.price));
    ui->lEditMinAmount->setText(ConvertData::toUiPrice(info.minAmount));
    ui->lEditTimeCreate->setText(QDateTime::fromTime_t(info.timeCreate).toString("dd.MM.yyyy hh:mm"));
    ui->lEditTimeExpiration->setText(timeExpiration.toString("dd.MM.yyyy hh:mm"));
    ui->tEditShortInfo->setText(info.shortInfo);
    ui->tEditDetails->setText(info.details);
}

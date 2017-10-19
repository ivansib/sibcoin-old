#include "offerdetailseditor.h"
#include "ui_offerdetailseditor.h"

OfferDetailsEditor::OfferDetailsEditor(DexDB *db, QDialog *parent) : QDialog(parent), ui(new Ui::OfferDetailsEditor)
{
    ui->setupUi(this);

    auto payments = db->getPaymentMethodsInfo();
    ui->cBoxPayment->addData(payments);

    auto countries = db->getCountriesInfo();
    ui->cBoxCountry->addData(countries);

    auto currencies = db->getCurrenciesInfo();
    ui->cBoxCurrency->addData(currencies);

    connect(ui->btnBox, &QDialogButtonBox::accepted, this, &OfferDetailsEditor::saveData);
    connect(ui->btnBox, &QDialogButtonBox::rejected, this, &OfferDetailsEditor::close);
}

OfferDetailsEditor::~OfferDetailsEditor()
{
    delete ui;
}

void OfferDetailsEditor::setOfferInfo(const QtOfferInfo &info)
{
    ui->lEditId->setText(info.idTransaction);
    ui->lEditHash->setText(info.hash);
    ui->cBoxCountry->setCurrentData(info.countryIso);
    ui->cBoxCurrency->setCurrentData(info.currencyIso);
    ui->cBoxPayment->setCurrentData(QString::number(info.paymentMethod));
    ui->lEditPrice->setText(QString::number(info.price));
    ui->lEditMinAmount->setText(QString::number(info.minAmount));
    ui->lEditTimeCreate->setText(QDateTime::fromTime_t(info.timeCreate).toString("dd.MM.yyyy hh:mm"));
    ui->dtEditExpiration->setDateTime(QDateTime::fromTime_t(info.timeExpiration));
    ui->tEditShortInfo->setText(info.shortInfo);
    ui->tEditDetails->setText(info.details);
}

void OfferDetailsEditor::saveData()
{
    QtOfferInfo info;
    info.idTransaction = ui->lEditId->text();
    info.hash = ui->lEditHash->text();
    info.countryIso = ui->cBoxCountry->currentData().toString();
    info.currencyIso = ui->cBoxCurrency->currentData().toString();
    info.paymentMethod = ui->cBoxPayment->currentData().toInt();
    info.price = ui->lEditPrice->text().toInt();
    info.minAmount = ui->lEditMinAmount->text().toInt();
    info.timeCreate = QDateTime::fromString(ui->lEditTimeCreate->text(), "dd.MM.yyyy hh:mm").toTime_t();
    info.timeExpiration = ui->dtEditExpiration->dateTime().toTime_t();
    info.shortInfo = ui->tEditShortInfo->toPlainText();
    info.details = ui->tEditDetails->toPlainText();

    Q_EMIT dataChanged(info);
    close();
}

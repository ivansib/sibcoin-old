#include <QDateTime>
#include "offerdetailseditor.h"
#include "ui_offerdetailseditor.h"

OfferDetailsEditor::OfferDetailsEditor(DexDB *db, QDialog *parent) : QDialog(parent), ui(new Ui::OfferDetailsEditor)
{
    ui->setupUi(this);

    auto payments = db->getPaymentMethodsInfo();
    ui->cBoxPayment->addData(payments, ComboBox::Editor);

    auto countries = db->getCountriesInfo();
    ui->cBoxCountry->addData(countries, ComboBox::Editor);

    auto currencies = db->getCurrenciesInfo(DexDB::Enabled);
    ui->cBoxCurrency->addData(currencies, ComboBox::Editor);

    connect(ui->btnBox, &QDialogButtonBox::accepted, this, &OfferDetailsEditor::saveData);
    connect(ui->btnBox, &QDialogButtonBox::rejected, this, &OfferDetailsEditor::close);
    connect(ui->sBoxExpiration, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &OfferDetailsEditor::changedTimeToExpiration);
    connect(ui->tEditShortInfo, &QTextEdit::textChanged, this, &OfferDetailsEditor::changedShortInfo);
}

OfferDetailsEditor::~OfferDetailsEditor()
{
    delete ui;
}

void OfferDetailsEditor::setOfferInfo(const QtOfferInfo &info)
{
    QDateTime timeExpiration = QDateTime::fromTime_t(info.timeCreate).addDays(info.timeToExpiration);

    ui->lEditId->setText(info.idTransaction);
    ui->lEditHash->setText(info.hash);
    ui->cBoxCountry->setCurrentData(info.countryIso);
    ui->cBoxCurrency->setCurrentData(info.currencyIso);
    ui->cBoxPayment->setCurrentData(QString::number(info.paymentMethod));
    ui->sBoxPrice->setValue(info.price);
    ui->sBoxMinAmount->setValue(info.minAmount);
    ui->lEditTimeCreate->setText(QDateTime::fromTime_t(info.timeCreate).toString("dd.MM.yyyy hh:mm"));
    ui->sBoxExpiration->setValue(info.timeToExpiration);
    ui->lEditTimeExpiration->setText(timeExpiration.toString("dd.MM.yyyy hh:mm"));
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
    info.price = ui->sBoxPrice->value();
    info.minAmount = ui->sBoxMinAmount->value();
    info.timeCreate = QDateTime::fromString(ui->lEditTimeCreate->text(), "dd.MM.yyyy hh:mm").toTime_t();
    info.timeToExpiration = ui->sBoxExpiration->value();
    info.shortInfo = ui->tEditShortInfo->toPlainText();
    info.details = ui->tEditDetails->toPlainText();

    Q_EMIT dataChanged(info);
    close();
}

void OfferDetailsEditor::changedTimeToExpiration(const int &i)
{
    QDateTime timeCreate = QDateTime::fromString(ui->lEditTimeCreate->text(), "dd.MM.yyyy hh:mm");
    QDateTime timeExpiration = timeCreate.addDays(i);
    ui->lEditTimeExpiration->setText(timeExpiration.toString("dd.MM.yyyy hh:mm"));
}

void OfferDetailsEditor::changedShortInfo()
{
    QString text = ui->tEditShortInfo->toPlainText();
    int size = text.size();

    if (size > 140) {
        ui->tEditShortInfo->setText(text.mid(0, 140));
        ui->tEditShortInfo->moveCursor(QTextCursor::End);
    }
}

#include <QDateTime>
#include "offerdetails.h"
#include "convertdata.h"

OfferDetails::OfferDetails(DexDB *db, const Type &type, QDialog *parent) : QDialog(parent), type(type)
{
    setupUi(this);

    auto payments = db->getPaymentMethodsInfo();
    cBoxPayment->addData(payments, ComboBox::Editor);

    auto countries = db->getCountriesInfo();
    cBoxCountry->addData(countries, ComboBox::Editor);

    auto currencies = db->getCurrenciesInfo(DexDB::Enabled);
    cBoxCurrency->addData(currencies, ComboBox::Editor);

    connect(btnBox, &QDialogButtonBox::accepted, this, &OfferDetails::saveData);
    connect(btnBox, &QDialogButtonBox::rejected, this, &OfferDetails::close);
    connect(sBoxExpiration, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &OfferDetails::changedTimeToExpiration);
    connect(tEditShortInfo, &QTextEdit::textChanged, this, &OfferDetails::changedShortInfo);

    initMode();
}

OfferDetails::~OfferDetails()
{
}

const void OfferDetails::initMode()
{
    if (type == Edit) {
        lOffer->setVisible(false);
        cBoxOffer->setVisible(false);
    } else {
        lIdTransaction->setVisible(false);
        lEditId->setVisible(false);
        lHash->setVisible(false);
        lEditHash->setVisible(false);

        lTimeCreate->setText(tr("Approximate Time Create:"));
        lInfoExpiration->setText(tr("Approximate Time Expiration:"));

        lEditTimeCreate->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));

        cBoxOffer->addItem(tr("Buy"));
        cBoxOffer->addItem(tr("Sell"));
    }
}

void OfferDetails::changedTimeToExpiration(const int &i)
{
    QDateTime timeCreate = QDateTime::fromString(lEditTimeCreate->text(), "dd.MM.yyyy hh:mm");
    QDateTime timeExpiration = timeCreate.addDays(i);
    lEditTimeExpiration->setText(timeExpiration.toString("dd.MM.yyyy hh:mm"));
}

void OfferDetails::changedShortInfo()
{
    QString text = tEditShortInfo->toPlainText();
    int size = text.size();

    if (size > 140) {
        tEditShortInfo->setText(text.mid(0, 140));
        tEditShortInfo->moveCursor(QTextCursor::End);
    }
}

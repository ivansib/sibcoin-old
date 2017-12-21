#include "offerdetails.h"
#include "convertdata.h"

OfferDetails::OfferDetails(DexDB *db, const Type &type, QDialog *parent) : QDialog(parent), db(db), type(type)
{
    setupUi(this);

    updateNavigationData();

    connect(btnSend, &QPushButton::clicked, this, &OfferDetails::sendData);
    connect(btnSaveDraft, &QPushButton::clicked, this, &OfferDetails::saveData);
    connect(btnCancel, &QPushButton::clicked, this, &OfferDetails::close);
    connect(sBoxExpiration, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &OfferDetails::changedTimeToExpiration);
    connect(tEditShortInfo, &QTextEdit::textChanged, this, &OfferDetails::changedShortInfo);

    initMode();
}

OfferDetails::~OfferDetails()
{
}

void OfferDetails::updateNavigationData()
{
    auto payments = db->getPaymentMethodsInfo();
    cBoxPayment->addData(payments, ComboBox::Editor);

    auto countries = db->getCountriesInfo();
    cBoxCountry->addData(countries, ComboBox::Editor);

    auto currencies = db->getCurrenciesInfo();
    cBoxCurrency->addData(currencies, ComboBox::Editor);
}

void OfferDetails::initMode()
{
    if (type == Edit) {
        delete lOffer;
        delete cBoxOffer;
    } else {
        delete lPubKey;
        delete lPubKeyView;
        delete lIdTransaction;
        delete lIdView;
        delete lHash;
        delete lHashView;
        delete lStatus;
        delete lStatusView;
        delete lOfferInfo;
        delete lOfferInfoView;
        delete lTimeCreate;
        delete lEditTimeCreate;

        setWindowTitle("Create Offer");
        isApproximateExpiration(true);

        cBoxOffer->addItem(tr("Buy"));
        cBoxOffer->addItem(tr("Sell"));
    }
}

void OfferDetails::isApproximateExpiration(const bool &b)
{
    if (b) {
        lInfoExpiration->setText(tr("Approximate Time Expiration:"));
    } else {
        lInfoExpiration->setText(tr("Time Expiration:"));
    }
}

void OfferDetails::changedTimeToExpiration(const int &i)
{
    QDateTime timeCreate;

    if (type == Edit) {
        timeCreate = QDateTime::fromString(lEditTimeCreate->text(), "dd.MM.yyyy hh:mm");
    } else {
        timeCreate = QDateTime::currentDateTime();
    }

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

#include "offerdetails.h"
#include "convertdata.h"

OfferDetails::OfferDetails(DexDB *db, const Type &type, QDialog *parent) : QDialog(parent), db(db), type(type)
{
    setupUi(this);

    updateNavigationData();

    connect(btnBox, &QDialogButtonBox::accepted, this, &OfferDetails::saveData);
    connect(btnBox, &QDialogButtonBox::rejected, this, &OfferDetails::close);
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

const void OfferDetails::initMode()
{
    if (type == Edit) {
        delete lOffer;
        delete cBoxOffer;
    } else {
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
        lInfoExpiration->setText(tr("Approximate Time Expiration:"));

        cBoxOffer->addItem(tr("Buy"));
        cBoxOffer->addItem(tr("Sell"));
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

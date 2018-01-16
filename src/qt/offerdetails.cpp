#include <QMessageBox>
#include "offerdetails.h"
#include "convertdata.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"

OfferDetails::OfferDetails(DexDB *db, QDialog *parent) : QDialog(parent), db(db), expirations(QList<int>() << 10 << 20 << 30)
{
}

OfferDetails::~OfferDetails()
{
}

void OfferDetails::setModel(WalletModel *model)
{
    this->model = model;
}

void OfferDetails::addBtnSend(QPushButton *btn)
{
    connect(btn, &QPushButton::clicked, this, &OfferDetails::sendData);
}

void OfferDetails::addBtnSaveDraft(QPushButton *btn)
{
    connect(btn, &QPushButton::clicked, this, &OfferDetails::saveData);
}

void OfferDetails::addBtnCancel(QPushButton *btn)
{
    connect(btn, &QPushButton::clicked, this, &OfferDetails::close);
}

void OfferDetails::addCBoxOffer(QComboBox *cBox)
{
    boxOffer = cBox;
}

void OfferDetails::addCBoxPayment(ComboBox *cBox)
{
    boxPayment = cBox;
}

void OfferDetails::addCBoxCountry(ComboBox *cBox)
{
    boxCountry = cBox;
}

void OfferDetails::addCBoxCurrency(ComboBox *cBox)
{
    boxCurrency = cBox;
}

void OfferDetails::addExpiration(QComboBox *cBox, QLabel *label)
{
    editTimeExpiration = label;

    connect(cBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OfferDetails::changedTimeToExpiration);
}

void OfferDetails::addLEditTransactionPrice(QLabel *label)
{
    editTransactionPrice = label;
}

void OfferDetails::addTEditShortInfo(QTextEdit *tEdit)
{
    editShortInfo = tEdit;

    connect(editShortInfo, &QTextEdit::textChanged, this, &OfferDetails::changedShortInfo);
}

void OfferDetails::updateNavigationData()
{
    boxOffer->addItems(typeOffers());

    auto payments = db->getPaymentMethodsInfo();
    boxPayment->addData(payments, ComboBox::Editor);

    auto countries = db->getCountriesInfo();
    boxCountry->addData(countries, ComboBox::Editor);

    auto currencies = db->getCurrenciesInfo();
    boxCurrency->addData(currencies, ComboBox::Editor);
}

QStringList OfferDetails::typeOffers() const
{
    QStringList l;
    l << tr("Buy") << tr("Sell");
    return l;
}

bool OfferDetails::confirmationSend()
{
    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send offer"),
        tr("Confirm send offer?"),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if(retval == QMessageBox::Yes) {
        return true;
    }

    return false;
}

CAmount OfferDetails::transactionPrice(const int &coef) const
{
    CAmount amount = PAYOFFER_RETURN_FEE + PAYOFFER_TX_FEE * coef;

    return amount;
}

void OfferDetails::changedTimeToExpiration(const int &i)
{
    QDateTime timeCreate = QDateTime::currentDateTime();
    QDateTime timeExpiration = timeCreate.addDays(expirations[i]);

    editTimeExpiration->setText(timeExpiration.toString("dd.MM.yyyy hh:mm"));

    CAmount amount = transactionPrice(i + 1);

    int nDisplayUnit = model->getOptionsModel()->getDisplayUnit();
    QString strAmount(BitcoinUnits::formatWithUnit(nDisplayUnit, amount, true, BitcoinUnits::separatorAlways));
    editTransactionPrice->setText(strAmount);
}

void OfferDetails::changedShortInfo()
{
    QString text = editShortInfo->toPlainText();
    int size = text.size();

    if (size > 140) {
        editShortInfo->setText(text.mid(0, 140));
        editShortInfo->moveCursor(QTextCursor::End);
    }
}

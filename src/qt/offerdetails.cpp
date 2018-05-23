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
    connect(btn, SIGNAL(clicked()), this, SLOT(sendData()));
}

void OfferDetails::addBtnSaveDraft(QPushButton *btn)
{
    connect(btn, SIGNAL(clicked()), this, SLOT(saveData()));
}

void OfferDetails::addBtnCancel(QPushButton *btn)
{
    connect(btn, SIGNAL(clicked()), this, SLOT(close()));
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

    connect(cBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changedTimeToExpiration(int)));
}

void OfferDetails::addLEditTransactionPrice(QLabel *label)
{
    editTransactionPrice = label;
}

void OfferDetails::addTEditShortInfo(QTextEdit *tEdit)
{
    editShortInfo = tEdit;

    connect(editShortInfo, SIGNAL(textChanged()), this, SLOT(changedShortInfo()));
}

void OfferDetails::addTEditDetailInfo(QTextEdit *tEdit)
{
    editDetailInfo = tEdit;

    connect(editDetailInfo, SIGNAL(textChanged()), this, SLOT(changedDetailInfo()));
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

void OfferDetails::messageSyncDexNotFinished()
{
    QMessageBox::StandardButton retval = QMessageBox::warning(this, tr("Warning"),
                                                              tr("Dex synchronization is not complete yet, you need to wait"));
}

CAmount OfferDetails::transactionPrice(const int &coef) const
{
    CAmount amount = PAYOFFER_RETURN_FEE + PAYOFFER_TX_FEE * coef;

    return amount;
}

void OfferDetails::changedTimeToExpiration(const int &i)
{
    QDateTime timeCreate = QDateTime::fromTime_t(GetAdjustedTime());
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

    if (size > DEX_SHORT_INFO_LENGTH) {
        editShortInfo->setText(text.mid(0, DEX_SHORT_INFO_LENGTH));
        editShortInfo->moveCursor(QTextCursor::End);
    }
}

void OfferDetails::changedDetailInfo()
{
    QString text = editDetailInfo->toPlainText();
    int size = text.size();

    if (size > DEX_DETAILS_LENGTH) {
        editDetailInfo->setText(text.mid(0, DEX_DETAILS_LENGTH));
        editDetailInfo->moveCursor(QTextCursor::End);
    }
}

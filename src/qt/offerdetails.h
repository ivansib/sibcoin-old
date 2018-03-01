#ifndef OFFERDETAILS_H
#define OFFERDETAILS_H

#include <QDialog>
#include <QDateTime>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QLabel>
#include "combobox.h"
#include "dex/db/dexdb.h"
#include "dex/dex.h"
#include "dto.h"
#include "amount.h"
#include "walletmodel.h"

using namespace dex;

class OfferDetails : public QDialog
{
    Q_OBJECT

public:
    OfferDetails(DexDB *db, QDialog *parent = nullptr);
    virtual ~OfferDetails();

    void setModel(WalletModel *model);

    void addBtnSend(QPushButton *btn);
    void addBtnSaveDraft(QPushButton *btn);
    void addBtnCancel(QPushButton *btn);

    void addCBoxOffer(QComboBox *cBox);
    void addCBoxPayment(ComboBox *cBox);
    void addCBoxCountry(ComboBox *cBox);
    void addCBoxCurrency(ComboBox *cBox);

    void addTEditShortInfo(QTextEdit *tEdit);

    void addExpiration(QComboBox *cBox, QLabel *label);

    void addLEditTransactionPrice(QLabel *label);

protected:
    DexDB *db;
    WalletModel *model;
    const QList<int> expirations;

    QStringList typeOffers() const;
    bool confirmationSend();

private:
    QComboBox *boxOffer;
    ComboBox *boxPayment;
    ComboBox *boxCountry;
    ComboBox *boxCurrency;
    QTextEdit *editShortInfo;

    QLabel *editTimeExpiration;
    QLabel *editTransactionPrice;

    CAmount transactionPrice(const int &coef) const;

public Q_SLOTS:
    void updateNavigationData();

protected Q_SLOTS:
    virtual void saveData() = 0;
    virtual void sendData() = 0;

    void changedTimeToExpiration(const int &i);

private Q_SLOTS:
    void changedShortInfo();

Q_SIGNALS:
    void dataSave(const QtMyOfferInfo &info);
    void dataSend(const QtMyOfferInfo &info);
};


#endif

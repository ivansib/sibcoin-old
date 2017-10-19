#ifndef TABLEOFFERSDIALOG_H
#define TABLEOFFERSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include "dex/dexdb.h"
#include "offermodel.h"
#include "offerdetails.h"

using namespace dex;

namespace Ui {
    class TableOffersDialog;
}

class TableOffersDialog : public QDialog
{
    Q_OBJECT

public:
    TableOffersDialog(DexDB *db, QDialog *parent = nullptr);
    virtual ~TableOffersDialog();

protected:
    OfferModel *pModel;
    DexDB *db;

    void init();
    QString currentCountry() const;
    QString currentCurrency() const;
    QString currentPayment() const;

private:
    Ui::TableOffersDialog *ui;

protected Q_SLOTS:
    virtual void clickedColumn(QModelIndex index) = 0;

private Q_SLOTS:
    void changedFilterCountryIso(const int &);
    void changedFilterCurrencyIso(const int &);
    void changedFilterPaymentMethod(const int &);
};

#endif

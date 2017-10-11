#ifndef TABLEOFFERSDIALOG_H
#define TABLEOFFERSDIALOG_H

#include <QDialog>
#include "dex/dexdb.h"
#include "offermodel.h"

using namespace dex;

namespace Ui {
    class TableOffersDialog;
}

class TableOffersDialog : public QDialog
{
    Q_OBJECT

public:
    TableOffersDialog(DexDB *db, QDialog *parent = nullptr);
    ~TableOffersDialog();

private:
    Ui::TableOffersDialog *ui;

    DexDB *db;
    OfferModel *pModel;

private Q_SLOTS:
    void changedFilterCountryIso(const int &);
    void changedFilterCurrencyIso(const int &);
    void changedFilterPaymentMethod(const int &);
};

#endif

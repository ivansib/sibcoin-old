#ifndef TABLEOFFERSDIALOG_H
#define TABLEOFFERSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSignalMapper>
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
    virtual ~TableOffersDialog();

protected:
    OfferModel *pModel;
    DexDB *db;

    void init();
    QString currentCountry() const;
    QString currentCurrency() const;
    QString currentPayment() const;
    int currentOfferIndex() const;

    void useMyOfferMode(const bool &b);

private:
    Ui::TableOffersDialog *ui;
    QSignalMapper *pMapper;
    void addButtons();
    void resizeColumns();

protected Q_SLOTS:
    virtual void openCreatorOffer();
    virtual void clickedButton(const int &index) = 0;

private Q_SLOTS:
    void changedFilterCountryIso(const int &);
    void changedFilterCurrencyIso(const int &);
    void changedFilterPaymentMethod(const int &);
    void changedFilterOfferType(const int &);
};

#endif

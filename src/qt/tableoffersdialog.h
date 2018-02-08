#ifndef TABLEOFFERSDIALOG_H
#define TABLEOFFERSDIALOG_H

#include "tableofferssubdialog.h"
#include "offermodelview.h"
#include "offermodeleditor.h"

template <class Offer>
class TableOffersDialog : public TableOffersSubDialog
{
public:
    TableOffersDialog(DexDB *db, OfferModel<Offer> *model, const int &columnBtn, QDialog *parent = nullptr);
    virtual ~TableOffersDialog();

protected:
    OfferModel<Offer> *pModel;

protected:
    virtual void changedFilterCountryIso(const int &);
    virtual void changedFilterCurrencyIso(const int &);
    virtual void changedFilterPaymentMethod(const int &);
    virtual void changedFilterOfferType(const int &);
};

#endif

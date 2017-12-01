#ifndef TABLEOFFERSVIEW_H
#define TABLEOFFERSVIEW_H

#include "tableoffersdialog.h"
#include "offerdetailsview.h"

class TableOffersView : public TableOffersDialog
{
    Q_OBJECT

public:
    enum TypeOffer {
        Buy,
        Sell
    };

    TableOffersView(DexDB *db, const TypeOffer &type, QDialog *parent = nullptr);
    ~TableOffersView();

private:
    OfferDetailsView *details;
    const TypeOffer type;

public Q_SLOTS:
   void updateData();

protected Q_SLOTS:
    virtual void clickedButton(const int &index);
};

#endif

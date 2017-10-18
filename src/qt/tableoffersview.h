#ifndef TABLEOFFERSVIEW_H
#define TABLEOFFERSVIEW_H

#include "tableoffersdialog.h"

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
    OfferDetails *details;

protected Q_SLOTS:
    virtual void clickedColumn(QModelIndex index);
};

#endif

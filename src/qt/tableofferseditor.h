#ifndef TABLEOFFERSEDITOR_H
#define TABLEOFFERSEDITOR_H

#include "tableoffersdialog.h"
#include "offerdetailseditor.h"

class TableOffersEditor : public TableOffersDialog
{
    Q_OBJECT

public:
    TableOffersEditor(DexDB *db, QDialog *parent = nullptr);
    ~TableOffersEditor();

private:
    OfferDetailsEditor *details;

    void updateData();

protected Q_SLOTS:
    virtual void clickedColumn(QModelIndex index);
    virtual void changedFilterOffer(const int &);

private Q_SLOTS:
    void changedRowData(const QtOfferInfo &info);
};

#endif

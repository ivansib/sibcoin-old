#ifndef TABLEOFFERSEDITOR_H
#define TABLEOFFERSEDITOR_H

#include "tableoffersdialog.h"
#include "offerdetailseditor.h"
#include "offerdetailscreator.h"

class TableOffersEditor : public TableOffersDialog
{
    Q_OBJECT

public:
    TableOffersEditor(DexDB *db, QDialog *parent = nullptr);
    ~TableOffersEditor();

private:
    OfferDetailsEditor *editor;
    OfferDetailsCreator *creator;

    void updateData();

protected Q_SLOTS:
    virtual void clickedButton(const int &index);
    virtual void openCreatorOffer();
    virtual void changedFilterOffer(const int &);

private Q_SLOTS:
    void changedRowData(const QtMyOfferInfo &info);

Q_SIGNALS:
    void dataChanged();
};

#endif

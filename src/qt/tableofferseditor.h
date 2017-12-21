#ifndef TABLEOFFERSEDITOR_H
#define TABLEOFFERSEDITOR_H

#include "tableoffersdialog.h"
#include "offerdetailseditor.h"
#include "offerdetailscreator.h"
#include "callbackdbforgui.h"

class TableOffersEditor : public TableOffersDialog<QtMyOfferInfo>
{
    Q_OBJECT

public:
    TableOffersEditor(DexDB *db, QDialog *parent = nullptr);
    ~TableOffersEditor();

private:
    OfferDetailsEditor *editor;
    OfferDetailsCreator *creator;
    CallBackDbForGui *callBack;

    void updateData();
    void saveMyOffer(const MyOfferInfo &info);

protected Q_SLOTS:
    virtual void clickedButton(const int &index);
    virtual void openCreatorOffer();
    virtual void updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);

private Q_SLOTS:
    void addOrEditMyOffer(const QtMyOfferInfo &info);
    void sendMyOffer(const QtMyOfferInfo &info);
};

#endif

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

    void setModel(WalletModel *model);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    OfferDetailsEditor *editor;
    OfferDetailsCreator *creator;
    CallBackDbForGui *callBack;
    WalletModel *model;

    void saveMyOffer(const MyOfferInfo &info);

public Q_SLOTS:
    void updateData();

protected Q_SLOTS:
    virtual void clickedButton(const int &index);
    virtual void openCreatorOffer();
    virtual void updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);

private Q_SLOTS:
    void addOrEditDraftMyOffer(const QtMyOfferInfo &info);
    void sendMyOffer(const QtMyOfferInfo &info);
    void deleteDraftData(const QtMyOfferInfo &info);
    void updateTable();
};

#endif

#ifndef TABLEOFFERSEDITOR_H
#define TABLEOFFERSEDITOR_H

#include "tableoffersdialog.h"
#include "offerdetailseditor.h"
#include "offerdetailscreator.h"
#include "callbackdbforgui.h"
#include "tableoffereditordelegate.h"
#include "controllerthreaddexdb.h"

class TableOffersEditor : public TableOffersDialog<QtMyOfferInfo>
{
    Q_OBJECT

public:
    TableOffersEditor(DexDB *db, QDialog *parent = nullptr);
    ~TableOffersEditor();

    void setModel(WalletModel *model);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual int countOffers() const override;

private:
    OfferDetailsEditor *editor;
    OfferDetailsCreator *creator;
    CallBackDbForGui *callBack;
    WalletModel *model;
    TableOfferEditorDelegate *pDelegate;
    ControllerThreadDexDB *threadDexDB;

    void saveMyOffer(const MyOfferInfo &info);

protected Q_SLOTS:
    virtual void openCreatorOffer() override;
    virtual void updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status) override;
    virtual void updateData() override;
    virtual void resizeTable() override;

    void clickedButtonEdit(const int &index);
    void clickedButtonDelete(const int &index);

private Q_SLOTS:
    void addOrEditDraftMyOffer(const QtMyOfferInfo &info);
    void sendMyOffer(const QtMyOfferInfo &info);
    void deleteOffer(const QtMyOfferInfo &info);
    void showMessageWarning(const QString &warning);
};

#endif

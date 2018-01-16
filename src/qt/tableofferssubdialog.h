#ifndef TABLEOFFERSSUBDIALOG_H
#define TABLEOFFERSSUBDIALOG_H

#include <QDialog>
#include <QComboBox>
#include "guiutil.h"
#include "dex/dexdb.h"
#include "tableofferdelegate.h"
#include "callbackdbforgui.h"
#include "commonsettingsforoffers.h"
#include "ui_tableoffersdialog.h"

using namespace dex;

class TableOffersSubDialog : public QDialog, public Ui::TableOffersDialog
{
    Q_OBJECT

public:
    TableOffersSubDialog(DexDB *db, const int &columnBtn, QDialog *parent = nullptr);
    virtual ~TableOffersSubDialog();

    void updateNavigationData();

protected:
    TableOfferDelegate *pDelegate;
    DexDB *db;
    CallBackDbForGui *callBack;
    GUIUtil::TableViewLastColumnResizingFixer *columnResizingFixer;

    void init();
    QString currentCountry() const;
    QString currentCurrency() const;
    QString currentPayment() const;
    int currentOfferIndex() const;

    void useMyOfferMode(const bool &b);

private:
    CommonSettingsForOffers *settings;

protected Q_SLOTS:
    virtual void openCreatorOffer();
    virtual void clickedButton(const int &index) = 0;
    virtual void updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);

    virtual void changedFilterCountryIso(const int &) = 0;
    virtual void changedFilterCurrencyIso(const int &) = 0;
    virtual void changedFilterPaymentMethod(const int &) = 0;
    virtual void changedFilterOfferType(const int &) = 0;

Q_SIGNALS:
    void navigationDataUpdate();
    void dataChanged();
};

#endif

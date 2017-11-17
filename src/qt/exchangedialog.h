#ifndef EXCHANGEDIALOG_H
#define EXCHANGEDIALOG_H

#include <QDialog>
#include "tableoffersview.h"
#include "tableofferseditor.h"
#include "settingsdialog.h"
#include "dex/dexdb.h"
#include "callbackdbforgui.h"

using namespace dex;

namespace Ui {
    class ExchangeDialog;
}

class ExchangeDialog : public QDialog
{
    Q_OBJECT

public:
    ExchangeDialog(QDialog *parent = nullptr);
    ~ExchangeDialog();

private:
    Ui::ExchangeDialog *ui;
    TableOffersView *tableBuy;
    TableOffersView *tableSell;
    TableOffersEditor *tableMyOffers;
    QWidget *widgetExchanges;
    SettingsDialog *settings;

    DexDB *db;
    CallBackDbForGui *callback;

private Q_SLOTS:
    void currentPageBuy();
    void currentPageSell();
    void currentPageMyOffers();
    void currentPageExchanges();
    void currentPageSettings();
    void updateData();
    void finishTableOperation(const TypeTable & table, const TypeTableOperation &operation, const StatusTableOperation &status);
};

#endif

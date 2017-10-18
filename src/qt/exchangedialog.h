#ifndef EXCHANGEDIALOG_H
#define EXCHANGEDIALOG_H

#include <QDialog>
#include "tableoffersview.h"
#include "tableofferseditor.h"
#include "dex/dexdb.h"

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
    QWidget *widgetSettings;

    DexDB *db;

private Q_SLOTS:
    void currentPageBuy();
    void currentPageSell();
    void currentPageMyOffers();
    void currentPageExchanges();
    void currentPageSettings();
};

#endif

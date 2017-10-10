#ifndef EXCHANGEDIALOG_H
#define EXCHANGEDIALOG_H

#include <QDialog>
#include "tableoffersdialog.h"
#include "dex/dexdb.h"

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
    TableOffersDialog *tableBuy;
    TableOffersDialog *tableSell;
    QWidget *widgetMyOffers;
    QWidget *widgetExchanges;
    QWidget *widgetSettings;

    dex::DexDB db;

private Q_SLOTS:
    void currentPageBuy();
    void currentPageSell();
    void currentPageMyOffers();
    void currentPageExchanges();
    void currentPageSettings();
};

#endif

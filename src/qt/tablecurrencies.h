#ifndef TABLECURRENCIES_H
#define TABLECURRENCIES_H

#include <QTableView>
#include "currenciesmodel.h"
#include <dex/dexdb.h>

using namespace dex;

class TableCurrencies : public QTableView
{
    Q_OBJECT

public:
    TableCurrencies(DexDB *db, QWidget *parent = nullptr);

    void saveData();
    void cancel();

private:
    DexDB *db;
    CurrenciesModel *model;

Q_SIGNALS:
    void dataChanged();
};

#endif

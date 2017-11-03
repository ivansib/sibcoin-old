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

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    DexDB *db;
    CurrenciesModel *model;

    bool isChangedData;

    void resizeColumn();

private Q_SLOTS:
    void changedData();

Q_SIGNALS:
    void dataChanged();
};

#endif

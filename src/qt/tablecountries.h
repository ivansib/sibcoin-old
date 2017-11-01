#ifndef TABLECOUNTRIES_H
#define TABLECOUNTRIES_H

#include <QTableView>
#include "countriesmodel.h"
#include "dex/dexdb.h"

using namespace dex;

class TableCountries : public QTableView
{
    Q_OBJECT

public:
    TableCountries(DexDB *db, QWidget *parent = nullptr);

    void saveData();
    void cancel();

private:
    DexDB *db;
    CountriesModel *model;

Q_SIGNALS:
    void dataChanged();
};

#endif

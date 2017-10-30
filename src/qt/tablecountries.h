#ifndef TABLECOUNTRIES_H
#define TABLECOUNTRIES_H

#include <QTableView>
#include "dex/dexdb.h"

using namespace dex;

class TableCountries : public QTableView
{
    Q_OBJECT

public:
    TableCountries(DexDB *db, QWidget *parent = nullptr);

private:
    DexDB *db;
};

#endif

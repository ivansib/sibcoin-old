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

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    DexDB *db;
    CountriesModel *model;

    bool isChangedData;

    void resizeColumn();

private Q_SLOTS:
    void changedData();

Q_SIGNALS:
    void dataChanged();
};

#endif

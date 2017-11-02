#ifndef FILTERLIST_H
#define FILTERLIST_H

#include <QWidget>
#include "dex/dexdb.h"

using namespace dex;

namespace Ui {
    class FilterList;
}

class FilterList : public QWidget
{
    Q_OBJECT

public:
    FilterList(DexDB *db, QWidget *parent = nullptr);
    ~FilterList();

private:
    Ui::FilterList *ui;
    DexDB *db;

private Q_SLOTS:
    void clikedAdd();
    void clickedDelete();
    void addItem(const QString &item);
};

#endif

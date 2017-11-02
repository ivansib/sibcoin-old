#ifndef FILTERLIST_H
#define FILTERLIST_H

#include <QWidget>
#include <QSet>
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

    void saveData();
    void cancel();

private:
    Ui::FilterList *ui;
    DexDB *db;

    void initList();

    QSet<QString> addItems;
    QSet<QString> deleteItems;

private Q_SLOTS:
    void clikedAdd();
    void clickedDelete();
    void addItem(const QString &item);

Q_SIGNALS:
    void dataChanged();
};

#endif

#include "tablecurrencies.h"
#include "convertdata.h"

TableCurrencies::TableCurrencies(DexDB *db, QWidget *parent) : QTableView(parent), db(db)
{
    QList<QtCurrencyInfo> currencies = ConvertData::toListQtCurrencyInfo(db->getCurrenciesInfo());
    model = new CurrenciesModel(currencies);

    setAlternatingRowColors(true);
    setDragDropOverwriteMode(true);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropMode(QAbstractItemView::DragDrop);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setAcceptDrops(true);

    setModel(model);
//    setItemDelegate(new CountriesDelegate());

    connect(model, &CurrenciesModel::dataChanged, this, &TableCurrencies::dataChanged);
}

void TableCurrencies::saveData()
{

}

void TableCurrencies::cancel()
{

}

#include "tablecurrencies.h"
#include "convertdata.h"
#include "settingsdelegate.h"

TableCurrencies::TableCurrencies(DexDB *db, QWidget *parent) : QTableView(parent), db(db)
{
    QList<QtCurrencyInfo> currencies = ConvertData::toListQtCurrencyInfo(db->getCurrenciesInfo());
    model = new CurrenciesModel(currencies);

    isChangedData = false;

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
    setItemDelegate(new SettingsDelegate(model->columnEdit()));

    connect(model, &CurrenciesModel::dataChanged, this, &TableCurrencies::changedData);
}

void TableCurrencies::saveData()
{
    if (isChangedData) {
        QList<QtCurrencyInfo> currencies = model->getCurrencies();

        for (int i = 0; i < currencies.size(); i++) {
            CurrencyInfo info = ConvertData::fromQtCurrencyInfo(currencies[i]);

            db->editCurrency(info.iso, info.enabled, i);
        }

        isChangedData = false;
    }
}

void TableCurrencies::cancel()
{
    if (isChangedData) {
        QList<QtCurrencyInfo> curencies = ConvertData::toListQtCurrencyInfo(db->getCurrenciesInfo());
        model->setCurrencies(curencies);

        isChangedData = false;
    }
}

void TableCurrencies::changedData()
{
    isChangedData = true;
    Q_EMIT dataChanged();
}

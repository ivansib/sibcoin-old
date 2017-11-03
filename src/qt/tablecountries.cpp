#include <QHeaderView>
#include <QScrollBar>
#include "tablecountries.h"
#include "convertdata.h"
#include "settingsdelegate.h"

TableCountries::TableCountries(DexDB *db, QWidget *parent) : QTableView(parent), db(db)
{
    QList<QtCountryInfo> countries = ConvertData::toListQtCountryInfo(db->getCountriesInfo());
    model = new CountriesModel(countries);

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

    resizeColumn();

    connect(model, &CountriesModel::dataChanged, this, &TableCountries::changedData);
}

void TableCountries::saveData()
{
    if (isChangedData) {
        QList<QtCountryInfo> countries = model->getCountries();

        for (int i = 0; i < countries.size(); i++) {
            CountryInfo info = ConvertData::fromQtCountryInfo(countries[i]);

            db->editCountry(info.iso, info.enabled, i);
        }

        isChangedData = false;
    }
}

void TableCountries::cancel()
{
    if (isChangedData) {
        QList<QtCountryInfo> countries = ConvertData::toListQtCountryInfo(db->getCountriesInfo());
        model->setCountries(countries);
        isChangedData = false;
    }
}

void TableCountries::resizeEvent(QResizeEvent *)
{
    resizeColumn();
}

void TableCountries::resizeColumn()
{
    int width = geometry().width();
    auto header = verticalHeader();
    int wH = header->sizeHint().width();
    int wS = verticalScrollBar()->sizeHint().width();
    int w1 = width - columnWidth(1) - columnWidth(2) - wH - wS - 5;
    setColumnWidth(0, w1);
}

void TableCountries::changedData()
{
    isChangedData = true;

    Q_EMIT dataChanged();
}

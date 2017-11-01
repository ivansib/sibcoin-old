#include "tablecountries.h"
#include "convertdata.h"
#include "countriesdelegate.h"

TableCountries::TableCountries(DexDB *db, QWidget *parent) : QTableView(parent), db(db)
{
    QList<QtCountryInfo> countries = ConvertData::toListQtCountryInfo(db->getCountriesInfo());
    model = new CountriesModel(countries);

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
    setItemDelegate(new CountriesDelegate(model->columnEdit()));

    connect(model, &CountriesModel::dataChanged, this, &TableCountries::dataChanged);
}

void TableCountries::saveData()
{
    QList<QtCountryInfo> countries = model->getCountries();

    for (int i = 0; i < countries.size(); i++) {
        CountryInfo info = ConvertData::fromQtCountryInfo(countries[i]);

        db->editCountry(info.iso, info.enabled, i);
    }
}

void TableCountries::cancel()
{
    QList<QtCountryInfo> countries = ConvertData::toListQtCountryInfo(db->getCountriesInfo());
    model->setCountries(countries);
}

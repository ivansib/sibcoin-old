#include "tablecountries.h"
#include "countriesmodel.h"
#include "convertdata.h"
#include "countriesdelegate.h"

TableCountries::TableCountries(DexDB *db, QWidget *parent) : QTableView(parent), db(db)
{
    QList<QtCountryInfo> countries = ConvertData::toListQtCountryInfo(db->getCountriesInfo());
    CountriesModel *model = new CountriesModel(countries);

    setModel(model);
    setItemDelegate(new CountriesDelegate());
}

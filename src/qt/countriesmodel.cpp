#include "countriesmodel.h"

CountriesModel::CountriesModel(const QList<QtCountryInfo> &countries, QObject *parent) : QAbstractTableModel(parent), countries(countries)
{
    listHead << tr("Name") << tr("ISO") << tr("Enabled");
}

QVariant CountriesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= countries.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return countries[index.row()].name;
        case 1:
            return countries[index.row()].iso;
        case 2:
            return countries[index.row()].enabled;
        default:
            break;
        }
    }

    return QVariant();
}

int CountriesModel::rowCount(const QModelIndex &parent) const
{
    return countries.size();
}

int CountriesModel::columnCount(const QModelIndex &parent) const
{
    return listHead.size();
}

QVariant CountriesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if(orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section + 1;
    } else if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return listHead.at(section);
    }

    return QVariant();
}

Qt::ItemFlags CountriesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractItemModel::flags(index);
}

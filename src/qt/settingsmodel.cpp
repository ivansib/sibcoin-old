#include "settingsmodel.h"

SettingsModel::SettingsModel(const int &columnEdit, QObject *parent) : QAbstractTableModel(parent), m_columnEdit(columnEdit)
{

}

int SettingsModel::columnCount(const QModelIndex &parent) const
{
    return listHead.size();
}

QVariant SettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled | QAbstractItemModel::flags(index);
    }

    if (index.column() == m_columnEdit) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions SettingsModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QStringList SettingsModel::mimeTypes() const
{
    QStringList types;
    types << "application/country.pos";
    return types;
}

QMimeData *SettingsModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    if (indexes.size() > 0 && indexes[0].isValid()) {
        stream << indexes[0].row();
    }

    mimeData->setData("application/country.pos", encodedData);
    return mimeData;
}

int SettingsModel::columnEdit() const
{
    return m_columnEdit;
}

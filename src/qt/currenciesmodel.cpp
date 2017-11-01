#include "currenciesmodel.h"

CurrenciesModel::CurrenciesModel(const QList<QtCurrencyInfo> &currencies, QObject *parent) : SettingsModel(3, parent), currencies(currencies)
{
    listHead << tr("Name") << tr("ISO") << tr("Symbol") << tr("Enabled");
}

void CurrenciesModel::setCurrencies(const QList<QtCurrencyInfo> &currencies)
{
    this->currencies = currencies;
    Q_EMIT layoutChanged();
}

QList<QtCurrencyInfo> CurrenciesModel::getCurrencies() const
{
    return currencies;
}

QVariant CurrenciesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= currencies.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return currencies[index.row()].name;
        case 1:
            return currencies[index.row()].iso;
        case 2:
            return currencies[index.row()].symbol;
        case 3:
            return currencies[index.row()].enabled ? QString("x") : QString("");
        default:
            break;
        }
    }

    return QVariant();
}

bool CurrenciesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    if (index.column() == 3) {
        currencies[index.row()].enabled = value.toBool();
        Q_EMIT dataChanged();
    }

    return true;
}

int CurrenciesModel::rowCount(const QModelIndex &parent) const
{
    return currencies.size();
}

bool CurrenciesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!data->hasFormat("application/country.pos")) {
        return false;
    }

    if (action == Qt::IgnoreAction) {
        return true;
    }

    int endRow = 0;
    if (!parent.isValid() && row < 0) {
        endRow = currencies.count();
    } else if (!parent.isValid()) {
        endRow = qMin(row, currencies.count());
    } else {
        endRow = parent.row();
    }

    QByteArray encodedData = data->data("application/country.pos");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        int pos;
        stream >> pos;
        beginMoveRows(parent, pos, pos, parent, endRow);
        currencies.move(pos, endRow);
        endMoveRows();
        endRow++;
    }

    Q_EMIT dataChanged();
    return true;
}

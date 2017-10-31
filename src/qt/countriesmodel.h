#ifndef COUNTRIESMODEL_H
#define COUNTRIESMODEL_H

#include <QAbstractTableModel>
#include "dto.h"

class CountriesModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CountriesModel(const QList<QtCountryInfo> &countries, QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QStringList listHead;

    QList<QtCountryInfo> countries;
};

#endif

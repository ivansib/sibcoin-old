#ifndef CURRENCIESMODEL_H
#define CURRENCIESMODEL_H

#include "settingsmodel.h"
#include "dto.h"

class CurrenciesModel : public SettingsModel
{
    Q_OBJECT

public:
    CurrenciesModel(const QList<QtCurrencyInfo> &currencies, QObject *parent = nullptr);

    void setCurrencies(const QList<QtCurrencyInfo> &currencies);
    QList<QtCurrencyInfo> getCurrencies() const;

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;

    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private:
    QList<QtCurrencyInfo> currencies;
};

#endif

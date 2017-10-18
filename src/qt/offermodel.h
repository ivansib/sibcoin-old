#ifndef OFFERMODEL_H
#define OFFERMODEL_H

#include <QAbstractTableModel>
#include "dto.h"

class OfferModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OfferModel(const QList<QtOfferInfo> &offers, QObject *parent = nullptr);
    ~OfferModel();

    void setFilterCountryIso(const QString &iso);
    void setFilterCurrencyIso(const QString &iso);
    void setFilterPaymentMethod(const uint8_t &payment);

    QtOfferInfo offerInfo(const int &row);

    virtual QVariant data(const QModelIndex &index, int role) const;

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:
    QStringList listHead;
    QList<QtOfferInfo> offers;
    QList<QtOfferInfo> offersView;

    QString countryIso;
    QString currencyIso;
    uint8_t paymentMethod;

    void filterOffers();
};

#endif

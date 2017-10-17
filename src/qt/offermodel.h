#ifndef OFFERMODEL_H
#define OFFERMODEL_H

#include <QAbstractTableModel>
#include "dex/dexdto.h"
#include <list>

using namespace dex;

class OfferModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OfferModel(const std::list<OfferInfo> &offers, QObject *parent = nullptr);
    ~OfferModel();

    void setFilterCountryIso(const std::string &iso);
    void setFilterCurrencyIso(const std::string &iso);
    void setFilterPaymentMethod(const uint8_t &payment);

    OfferInfo offerInfo(const int &row);

    virtual QVariant data(const QModelIndex &index, int role) const;

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:
    QStringList listHead;
    QList<OfferInfo> offers;
    QList<OfferInfo> offersView;

    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;

    void filterOffers();
};

#endif

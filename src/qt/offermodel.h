#ifndef OFFERMODEL_H
#define OFFERMODEL_H

#include <QAbstractTableModel>
#include "dto.h"

class OfferModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OfferModel(QObject *parent = nullptr);
    ~OfferModel();

    void setOffers(const QList<QtOfferInfo> &offers);
    void setOffers(const QList<QtMyOfferInfo> &offers);
    void setFilterCountryIso(const QString &iso);
    void setFilterCurrencyIso(const QString &iso);
    void setFilterPaymentMethod(const uint8_t &payment);
    void setFilterTypeOffer(const int &typeOffer);
    int rows() const;

    QtOfferInfo offerInfo(const int &row);
    QtMyOfferInfo myOfferInfo(const int &row);

    virtual QVariant data(const QModelIndex &index, int role) const;

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:
    enum Type {
        Offer,
        MyOffer
    };

    QStringList listHead;
    QList<QtMyOfferInfo> offers;
    QList<QtMyOfferInfo> offersView;

    Type type;

    QString countryIso;
    QString currencyIso;
    uint8_t paymentMethod;
    int typeOffer;

    void filterOffers();
};

#endif

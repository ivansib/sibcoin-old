#ifndef OFFERMODEL_H
#define OFFERMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include "commonsettingsforoffers.h"
#include "dto.h"

Q_DECLARE_METATYPE(QList<QPersistentModelIndex>)
Q_DECLARE_METATYPE(QAbstractItemModel::LayoutChangeHint)

template <class Offer>
class OfferModel : public QAbstractTableModel
{

public:
    OfferModel(QObject *parent = nullptr);
    virtual ~OfferModel();

    void setOffers(const QList<Offer> &offers);

    Offer offerInfo(const int &row);

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
    QStringList listHead;

    QList<Offer> offers;
    QList<Offer> offersView;
    CommonSettingsForOffers *settings;


    QString countryIso;
    QString currencyIso;
    uint8_t paymentMethod;
    int typeOffer;
};

#endif

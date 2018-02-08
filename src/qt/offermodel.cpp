#include "offermodel.h"
#include "convertdata.h"

template <class Offer>
OfferModel<Offer>::OfferModel(QObject *parent) : QAbstractTableModel(parent)
{
}

template <class Offer>
OfferModel<Offer>::~OfferModel()
{

}

template <class Offer>
void OfferModel<Offer>::setOffers(const QList<Offer> &offers)
{
    this->offers = offers;
    offersView = this->offers;

    filterOffers();

    Q_EMIT layoutChanged();
}

template <class Offer>
void OfferModel<Offer>::setFilterCountryIso(const QString &iso)
{
    countryIso = iso;
    filterOffers();
}

template <class Offer>
void OfferModel<Offer>::setFilterCurrencyIso(const QString &iso)
{
    currencyIso = iso;
    filterOffers();
}

template <class Offer>
void OfferModel<Offer>::setFilterPaymentMethod(const uint8_t &payment)
{
    paymentMethod = payment;
    filterOffers();
}

template <class Offer>
void OfferModel<Offer>::setFilterTypeOffer(const int &typeOffer)
{
    this->typeOffer = typeOffer;
    filterOffers();
}

template <class Offer>
int OfferModel<Offer>::rows() const
{
    return offersView.size();
}

template <class Offer>
Offer OfferModel<Offer>::offerInfo(const int &row)
{
    if (row < offersView.size()) {
        return offersView[row];
    }

    return Offer();
}

template <class Offer>
int OfferModel<Offer>::rowCount(const QModelIndex &parent) const
{
    return offersView.size();
}

template <class Offer>
int OfferModel<Offer>::columnCount(const QModelIndex &parent) const
{
    return listHead.size();
}

template <class Offer>
QVariant OfferModel<Offer>::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return listHead.at(section);
    }

    return QVariant();
}

template class OfferModel<QtMyOfferInfo>;
template class OfferModel<QtOfferInfo>;

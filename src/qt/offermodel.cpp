#include "offermodel.h"
#include "convertdata.h"

template <class Offer>
OfferModel<Offer>::OfferModel(QObject *parent) : QAbstractTableModel(parent)
{
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");

#if QT_VERSION > 0x050000
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
#endif

    settings = CommonSettingsForOffers::instance();
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

    Q_EMIT layoutChanged();
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
    if (!settings->getShowMaxRowsTables() && settings->getNumRowsTables() < offersView.size()) {
        return settings->getNumRowsTables();
    }

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

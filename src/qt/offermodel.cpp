#include "offermodel.h"
#include "convertdata.h"

OfferModel::OfferModel(const TypeTable &type, QObject *parent) : QAbstractTableModel(parent)
{
    this->type = type;

    listHead << tr("Price") << tr("Short Info") << tr("Min Amount");

    if (type == Offer) {
        listHead << tr("More Details");
    } else {
        listHead << tr("Edit");
    }
}

OfferModel::~OfferModel()
{

}

void OfferModel::setOffers(const QList<QtOfferInfo> &offers)
{
    QList<QtMyOfferInfo> myOffers;

    for (QtOfferInfo item : offers) {
        QtMyOfferInfo myItem;
        myItem.setOfferInfo(item);

        myOffers << myItem;
    }

    this->offers = myOffers;
    offersView = this->offers;

    filterOffers();

    Q_EMIT layoutChanged();
}

void OfferModel::setOffers(const QList<QtMyOfferInfo> &offers)
{
    this->offers = offers;
    offersView = this->offers;

    filterOffers();

    Q_EMIT layoutChanged();
}

void OfferModel::setFilterCountryIso(const QString &iso)
{
    countryIso = iso;
    filterOffers();
}

void OfferModel::setFilterCurrencyIso(const QString &iso)
{
    currencyIso = iso;
    filterOffers();
}

void OfferModel::setFilterPaymentMethod(const uint8_t &payment)
{
    paymentMethod = payment;
    filterOffers();
}

void OfferModel::setFilterTypeOffer(const int &typeOffer)
{
    this->typeOffer = typeOffer;
    filterOffers();
}

int OfferModel::rows() const
{
    return offersView.size();
}

QtOfferInfo OfferModel::offerInfo(const int &row)
{
    if (row < offersView.size()) {
        return offersView[row];
    }

    return QtOfferInfo();
}

QtMyOfferInfo OfferModel::myOfferInfo(const int &row)
{
    if (row < offersView.size()) {
        return offersView[row];
    }

    return QtMyOfferInfo();
}

QVariant OfferModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= offersView.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return ConvertData::toUiPrice(offersView[index.row()].price);
        case 1:
            return offersView[index.row()].shortInfo;
        case 2:
            return ConvertData::toUiPrice(offersView[index.row()].minAmount);
        case 3:
            if (type == Offer) {
                return tr("More Details");
            } else {
                return tr("Edit");
            }
        default:
            break;
        }
    }

    return QVariant();
}

int OfferModel::rowCount(const QModelIndex &parent) const
{
    return offersView.size();
}

int OfferModel::columnCount(const QModelIndex &parent) const
{
    return listHead.size();
}

QVariant OfferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return listHead.at(section);
    }

    return QVariant();
}

Qt::ItemFlags OfferModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (index.column() == 3) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    } else {
        return QAbstractItemModel::flags(index);
    }
}

void OfferModel::sort(int column, Qt::SortOrder order)
{
    if (order == Qt::AscendingOrder) {
        switch (column) {
        case 0:
            qSort(offersView.begin(), offersView.end(), [](QtOfferInfo& a, QtOfferInfo& b) { return a.price < b.price; } );
            break;
        case 1:
            qSort(offersView.begin(), offersView.end(), [](QtOfferInfo& a, QtOfferInfo& b) { return a.shortInfo < b.shortInfo; } );
            break;
        case 2:
            qSort(offersView.begin(), offersView.end(), [](QtOfferInfo& a, QtOfferInfo& b) { return a.minAmount < b.minAmount; } );
            break;
        default:
            break;
        }
    } else {
        switch (column) {
        case 0:
            qSort(offersView.begin(), offersView.end(), [](QtOfferInfo& a, QtOfferInfo& b) { return a.price > b.price; } );
            break;
        case 1:
            qSort(offersView.begin(), offersView.end(), [](QtOfferInfo& a, QtOfferInfo& b) { return a.shortInfo > b.shortInfo; } );
            break;
        case 2:
            qSort(offersView.begin(), offersView.end(), [](QtOfferInfo& a, QtOfferInfo& b) { return a.minAmount > b.minAmount; } );
            break;
        default:
            break;
        }
    }

    Q_EMIT layoutChanged();
}

void OfferModel::filterOffers()
{
    offersView.clear();
    for (auto item : offers) {
        bool b1 = (paymentMethod == item.paymentMethod || paymentMethod == 0);
        bool b2 = countryIso == item.countryIso || countryIso == "";
        bool b3 = currencyIso == item.currencyIso || currencyIso == "";

        bool b4 = false;
        if (type == Offer) {
            b4 = true;
        } else {
            if (item.type == TypeOffer::Buy && typeOffer == 1) {
                b4 = true;
            } else if (item.type == TypeOffer::Sell && typeOffer == 2) {
                b4 = true;
            } else if (typeOffer == 0) {
                b4 = true;
            }
        }

        if (b1 && b2 && b3 && b4) {
            offersView << item;
        }
    }

    Q_EMIT layoutChanged();
}

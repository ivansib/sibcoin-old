#include "offermodelview.h"
#include "convertdata.h"

OfferModelView::OfferModelView(QObject *parent) : OfferModel(parent)
{
    listHead << tr("Price") << tr("Short Info") << tr("Min Amount") << tr("More Details");
}

QVariant OfferModelView::data(const QModelIndex &index, int role) const
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
            return tr("More Details");
        default:
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags OfferModelView::flags(const QModelIndex &index) const
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

void OfferModelView::sort(int column, Qt::SortOrder order)
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

void OfferModelView::filterOffers()
{
    offersView.clear();
    for (auto item : offers) {
        bool b1 = (paymentMethod == item.paymentMethod || paymentMethod == 0);
        bool b2 = countryIso == item.countryIso || countryIso == "all";
        bool b3 = currencyIso == item.currencyIso || currencyIso == "all";

        if (b1 && b2 && b3) {
            offersView << item;
        }
    }

    Q_EMIT layoutChanged();
}

#include "offermodeleditor.h"
#include "convertdata.h"

OfferModelEditor::OfferModelEditor(QObject *parent) : OfferModel(parent)
{
    listHead << tr("Status") << tr("Price") << tr("Short Info") << tr("Min Amount") << tr("Edit");
}

QVariant OfferModelEditor::data(const QModelIndex &index, int role) const
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
            return status(offersView[index.row()].status);
        case 1:
            return ConvertData::toUiPrice(offersView[index.row()].price);
        case 2:
            return offersView[index.row()].shortInfo;
        case 3:
            return ConvertData::toUiPrice(offersView[index.row()].minAmount);
        case 4:
            return tr("Edit");
        default:
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags OfferModelEditor::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsEnabled;
    }

    if (index.column() == 4) {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    } else {
        return QAbstractItemModel::flags(index);
    }
}

void OfferModelEditor::sort(int column, Qt::SortOrder order)
{
    if (order == Qt::AscendingOrder) {
        switch (column) {
        case 0:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.status < b.status; } );
            break;
        case 1:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.price < b.price; } );
            break;
        case 2:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.shortInfo < b.shortInfo; } );
            break;
        case 3:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.minAmount < b.minAmount; } );
            break;
        default:
            break;
        }
    } else {
        switch (column) {
        case 0:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.status > b.status; } );
            break;
        case 1:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.price > b.price; } );
            break;
        case 2:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.shortInfo > b.shortInfo; } );
            break;
        case 3:
            qSort(offersView.begin(), offersView.end(), [](QtMyOfferInfo& a, QtMyOfferInfo& b) { return a.minAmount > b.minAmount; } );
            break;
        default:
            break;
        }
    }

    Q_EMIT layoutChanged();
}

void OfferModelEditor::filterOffers()
{
    offersView.clear();
    for (auto item : offers) {
        bool b1 = (paymentMethod == item.paymentMethod || paymentMethod == 0);
        bool b2 = countryIso == item.countryIso || countryIso == "";
        bool b3 = currencyIso == item.currencyIso || currencyIso == "";

        bool b4 = false;

        if (item.type == TypeOffer::Buy && typeOffer == 1) {
            b4 = true;
        } else if (item.type == TypeOffer::Sell && typeOffer == 2) {
            b4 = true;
        } else if (typeOffer == 0) {
            b4 = true;
        }

        if (b1 && b2 && b3 && b4) {
            offersView << item;
        }
    }

    Q_EMIT layoutChanged();
}

QString OfferModelEditor::status(const StatusOffer &s) const
{
    QString str;

    switch (s) {
    case Active:
        str = tr("Active");
        break;
    case Draft:
        str = tr("Draft");
        break;
    case Expired:
        str = tr("Expired");
        break;
    case Cancelled:
        str = tr("Cancelled");
        break;
    case Suspended:
        str = tr("Suspended");
        break;
    case Unconfirmed:
        str = tr("Unconfirmed");
        break;
    default:
        break;
    }

    return str;
}

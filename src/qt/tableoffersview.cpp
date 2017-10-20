#include "tableoffersview.h"
#include "convertdata.h"

TableOffersView::TableOffersView(DexDB *db, const TableOffersView::TypeOffer &type, QDialog *parent) : TableOffersDialog(db, parent)
{
    details = new OfferDetails(db, this);

    QList<QtOfferInfo> offers;
    if (type == Buy) {
        offers = ConvertData::toListQtOfferInfo(db->getOffersBuy());
    } else {
        offers = ConvertData::toListQtOfferInfo(db->getOffersSell());
    }

    pModel->setOffers(offers);

    init();
}

TableOffersView::~TableOffersView()
{
}

void TableOffersView::clickedColumn(QModelIndex index)
{
    if (index.column() == 3) {
        QtOfferInfo info = pModel->offerInfo(index.row());
        QString country = currentCountry();
        QString currency = currentCurrency();
        QString payment = currentPayment();
        details->setOfferInfo(info);
        details->show();
    }
}

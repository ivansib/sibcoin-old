#include "tableoffersview.h"
#include "convertdexdata.h"

TableOffersView::TableOffersView(DexDB *db, const TableOffersView::TypeOffer &type, QDialog *parent) : TableOffersDialog(db, parent)
{
    details = new OfferDetails(this);

    QList<QtOfferInfo> offers;
    if (type == Buy) {
        offers = ConvertDexData::toListQtOfferInfo(db->getOffersBuy());
    } else {
        offers = ConvertDexData::toListQtOfferInfo(db->getOffersSell());
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
        details->setOfferInfo(info, country, currency, payment);
        details->show();
    }
}

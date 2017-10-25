#include "tableoffersview.h"
#include "convertdata.h"

TableOffersView::TableOffersView(DexDB *db, const TableOffersView::TypeOffer &type, QDialog *parent) : TableOffersDialog(db, parent), db(db), type(type)
{
    details = new OfferDetailsView(db, this);

    updateData();
}

TableOffersView::~TableOffersView()
{
}

void TableOffersView::updateData()
{
    QList<QtOfferInfo> offers;
    if (type == Buy) {
        offers = ConvertData::toListQtOfferInfo(db->getOffersBuy());
    } else {
        offers = ConvertData::toListQtOfferInfo(db->getOffersSell());
    }

    pModel->setOffers(offers);

    init();
}

void TableOffersView::clickedButton(const int &index)
{
    QtOfferInfo info = pModel->offerInfo(index);
    details->setOfferInfo(info);
    details->show();
}

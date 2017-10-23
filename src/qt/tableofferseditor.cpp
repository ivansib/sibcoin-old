#include "tableofferseditor.h"
#include "convertdata.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent) : TableOffersDialog(db, parent)
{
    details = new OfferDetailsEditor(db, this);

    updateData();

    connect(details,  &OfferDetailsEditor::dataChanged, this, &TableOffersEditor::changedRowData);

    useOfferSort(true);
    init();
}

TableOffersEditor::~TableOffersEditor()
{

}

void TableOffersEditor::updateData()
{
    QList<QtOfferInfo> offers;
    if (currentOfferIndex() == 1) {
        offers = ConvertData::toListQtOfferInfo(db->getOffersBuy());
    } else if (currentOfferIndex() == 2) {
        offers = ConvertData::toListQtOfferInfo(db->getOffersSell());
    } else {
        offers = ConvertData::toListQtOfferInfo(db->getOffersBuy());
        offers.append(ConvertData::toListQtOfferInfo(db->getOffersSell()));
    }

    pModel->setOffers(offers);
}

void TableOffersEditor::clickedColumn(QModelIndex index)
{
    if (index.column() == 3) {
        QtOfferInfo info = pModel->offerInfo(index.row());
        details->setOfferInfo(info);
        details->show();
    }
}

void TableOffersEditor::changedFilterOffer(const int &)
{
    updateData();
}

void TableOffersEditor::changedRowData(const QtOfferInfo &info)
{
    OfferInfo offer = ConvertData::fromQtOfferInfo(info);
    db->editOfferBuy(offer);
    updateData();
}

#include "tableofferseditor.h"
#include "convertdata.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent) : TableOffersDialog(db, parent)
{
    details = new OfferDetailsEditor(db, this);

    updateData();

    connect(details,  &OfferDetailsEditor::dataChanged, this, &TableOffersEditor::changedRowData);

    init();
}

TableOffersEditor::~TableOffersEditor()
{

}

void TableOffersEditor::updateData()
{
    QList<QtOfferInfo> offers = ConvertData::toListQtOfferInfo(db->getOffersBuy());
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

void TableOffersEditor::changedRowData(const QtOfferInfo &info)
{
    OfferInfo offer = ConvertData::fromQtOfferInfo(info);
    db->editOfferBuy(offer);
    updateData();
}

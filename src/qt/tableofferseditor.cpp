#include "tableofferseditor.h"
#include "convertdexdata.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent) : TableOffersDialog(db, parent)
{
    details = new OfferDetailsEditor(db, this);

    QList<QtOfferInfo> offers = ConvertDexData::toListQtOfferInfo(db->getOffersBuy());
    pModel->setOffers(offers);

    init();
}

TableOffersEditor::~TableOffersEditor()
{

}

void TableOffersEditor::clickedColumn(QModelIndex index)
{
    if (index.column() == 3) {
        QtOfferInfo info = pModel->offerInfo(index.row());
        details->setOfferInfo(info);
        details->show();
    }
}

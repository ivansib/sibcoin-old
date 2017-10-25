#include "tableofferseditor.h"
#include "convertdata.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent) : TableOffersDialog(db, parent)
{
    editor = new OfferDetailsEditor(db, this);
    creator = new OfferDetailsCreator(db, this);

    updateData();

    connect(editor,  &OfferDetails::dataChanged, this, &TableOffersEditor::changedRowData);

    useMyOfferMode(true);
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

void TableOffersEditor::clickedButton(const int &index)
{
    QtOfferInfo info = pModel->offerInfo(index);
    editor->setOfferInfo(info);
    editor->show();
}

void TableOffersEditor::openCreatorOffer()
{
    creator->show();
}

void TableOffersEditor::changedFilterOffer(const int &)
{
    updateData();
}

void TableOffersEditor::changedRowData(const QtOfferInfo &info)
{
    OfferInfo offer = ConvertData::fromQtOfferInfo(info);

    if (db->isExistOfferBuy(offer.idTransaction)) {
        db->editOfferBuy(offer);
    } else if (db->isExistOfferSell(offer.idTransaction)) {
        db->editOfferSell(offer);
    }

    updateData();
    Q_EMIT dataChanged();
}

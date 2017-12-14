#include <QMessageBox>
#include "tableofferseditor.h"
#include "convertdata.h"
#include "net.h"
#include "../dex/dex.h"
#include "../dex/dexmanager.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent) : TableOffersDialog(db, parent)
{
    editor = new OfferDetailsEditor(db, this);
    creator = new OfferDetailsCreator(db, this);

    updateData();

    connect(editor, &OfferDetails::dataChanged, this, &TableOffersEditor::changedRowData);
    connect(creator, &OfferDetails::dataChanged, this, &TableOffersEditor::createNewOffer);

    connect(this, &TableOffersEditor::navigationDataUpdate, editor, &OfferDetailsEditor::updateNavigationData);
    connect(this, &TableOffersEditor::navigationDataUpdate, creator, &OfferDetailsEditor::updateNavigationData);

    useMyOfferMode(true);
    init();
}

TableOffersEditor::~TableOffersEditor()
{

}

void TableOffersEditor::updateData()
{
    QList<QtMyOfferInfo> offers = ConvertData::toListQtMyOfferInfo(db->getMyOffers());
    pModel->setOffers(offers);
}

void TableOffersEditor::clickedButton(const int &index)
{
    QtMyOfferInfo info = pModel->myOfferInfo(index);
    editor->setOfferInfo(info);
    editor->show();
}

void TableOffersEditor::openCreatorOffer()
{
    creator->initData();
    creator->show();
}

void TableOffersEditor::changedRowData(const QtMyOfferInfo &info)
{
    MyOfferInfo offer = ConvertData::fromQtMyOfferInfo(info);

    db->editMyOffer(offer);
}

void TableOffersEditor::createNewOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo offer = ConvertData::fromQtMyOfferInfo(info);
    CDex dex;

    dex.CreateOffer(offer);

    std::string error;
    uint256 tx;
    if (dex.PayForOffer(tx, error)) {
        dexman.sendOffer(dex.offer);

        offer.setOfferInfo(dex.offer);
        offer.status = Active;
        db->addMyOffer(offer);
        if (dex.offer.isBuy()) {
            db->addOfferBuy(dex.offer);
        }
        if (dex.offer.isSell()) {
            db->addOfferSell(dex.offer);
        }
    } else {
        offer.setOfferInfo(dex.offer);
        offer.status = Draft;
        db->addMyOffer(offer);

        QMessageBox::warning(this, tr("Warning"), tr(error.c_str()));
    }
}

void TableOffersEditor::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    if (table == MyOffers && (operation == Add || operation == Edit) && status == Ok) {
        updateData();
        Q_EMIT dataChanged();
    }
}

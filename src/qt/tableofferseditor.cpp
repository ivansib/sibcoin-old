#include <QMessageBox>
#include "tableofferseditor.h"
#include "convertdata.h"
#include "net.h"
#include "../dex/dex.h"
#include "../dex/dexmanager.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent) : TableOffersDialog(db, new OfferModelEditor(), 4, parent)
{
    editor = new OfferDetailsEditor(db, this);
    creator = new OfferDetailsCreator(db, this);

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 150);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 150);
    tableView->setColumnWidth(4, 150);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(tableView, 120, 23);

    updateData();

    connect(editor, &OfferDetailsEditor::dataSave, this, &TableOffersEditor::addOrEditMyOffer);
    connect(editor, &OfferDetailsEditor::dataSend, this, &TableOffersEditor::sendMyOffer);
    connect(editor, &OfferDetailsEditor::draftDataDelete, this, &TableOffersEditor::deleteDraftData);

    connect(creator, &OfferDetailsCreator::dataSave, this, &TableOffersEditor::addOrEditMyOffer);
    connect(creator, &OfferDetailsCreator::dataSend, this, &TableOffersEditor::sendMyOffer);

    connect(this, &TableOffersEditor::navigationDataUpdate, editor, &OfferDetailsEditor::updateNavigationData);
    connect(this, &TableOffersEditor::navigationDataUpdate, creator, &OfferDetailsCreator::updateNavigationData);

    useMyOfferMode(true);
    init();
}

TableOffersEditor::~TableOffersEditor()
{

}

void TableOffersEditor::setModel(WalletModel *model)
{
    this->model = model;
    creator->setModel(model);
    editor->setModel(model);
}

void TableOffersEditor::updateData()
{
    QList<QtMyOfferInfo> offers = ConvertData::toListQtMyOfferInfo(db->getMyOffers());
    pModel->setOffers(offers);
}

void TableOffersEditor::saveMyOffer(const MyOfferInfo &info)
{
    if (db->isExistMyOfferByHash(info.hash)) {
        db->editMyOffer(info);
    } else {
        db->addMyOffer(info);
    }
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

void TableOffersEditor::addOrEditMyOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo offer = ConvertData::fromQtMyOfferInfo(info);
    offer.status = Draft;

    CDexOffer dexOffer;
    uint256 oldHash = offer.hash;
    dexOffer.Create(offer);

    if (!oldHash.IsNull() && oldHash != dexOffer.hash) {
        db->deleteMyOfferByHash(oldHash);
    }

    offer.setOfferInfo(dexOffer);

    saveMyOffer(offer);
}

void TableOffersEditor::sendMyOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo myOffer = ConvertData::fromQtMyOfferInfo(info);

    CDexOffer dexOffer;

    if (myOffer.status == Indefined || myOffer.status == Draft) {
        uint256 oldHash = myOffer.hash;
        dexOffer.Create(myOffer);

        if (oldHash != dexOffer.hash) {
            db->deleteMyOfferByHash(oldHash);
        }
    } else if (myOffer.status == Active) {
        myOffer.editingVersion++;

        dexOffer = CDexOffer(myOffer);
    }

    CDex dex(dexOffer);

    std::string error;
    uint256 tx;

    if (!dexOffer.idTransaction.IsNull()) {
        dexman.sendEditedOffer(dex.offer);

        if (dex.offer.isBuy()) {
            db->editOfferBuy(dex.offer);
        }
        if (dex.offer.isSell()) {
            db->editOfferSell(dex.offer);
        }
    } else if (dex.PayForOffer(tx, error)) {
        dexman.sendNewOffer(dex.offer);

        myOffer.setOfferInfo(dex.offer);
        myOffer.status = Active;
        if (dex.offer.isBuy()) {
            db->addOfferBuy(dex.offer);
        }
        if (dex.offer.isSell()) {
            db->addOfferSell(dex.offer);
        }
    } else {
        myOffer.setOfferInfo(dex.offer);
        myOffer.status = Draft;

        QMessageBox::warning(this, tr("Warning"), tr(error.c_str()));
    }

    saveMyOffer(myOffer);
}

void TableOffersEditor::deleteDraftData(const QtMyOfferInfo &info)
{
    MyOfferInfo myOffer = ConvertData::fromQtMyOfferInfo(info);

    if (myOffer.status == Draft) {
        db->deleteMyOfferByHash(myOffer.hash);
    }
}

void TableOffersEditor::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    if (table == MyOffers && (operation == Add || operation == Edit || operation == Delete) && status == Ok) {
        updateData();
        Q_EMIT dataChanged();
    }
}

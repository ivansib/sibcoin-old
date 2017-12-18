#include <QMessageBox>
#include "tableofferseditor.h"
#include "convertdata.h"
#include "net.h"
#include "../dex/dex.h"
#include "../dex/dexmanager.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent) : TableOffersDialog(db, OfferModel::MyOffer, parent)
{
    editor = new OfferDetailsEditor(db, this);
    creator = new OfferDetailsCreator(db, this);

    updateData();

    connect(editor, &OfferDetails::dataSave, this, &TableOffersEditor::addOrEditMyOffer);
    connect(editor, &OfferDetails::dataSend, this, &TableOffersEditor::sendMyOffer);

    connect(creator, &OfferDetails::dataSave, this, &TableOffersEditor::addOrEditMyOffer);
    connect(creator, &OfferDetails::dataSend, this, &TableOffersEditor::sendMyOffer);

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

    if (info.hash == "") {
        CDex dex;
        dex.CreateOffer(offer);
        offer.setOfferInfo(dex.offer);
    }

    saveMyOffer(offer);
}

void TableOffersEditor::sendMyOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo myOffer = ConvertData::fromQtMyOfferInfo(info);

    CDexOffer dexOffer;

    if (myOffer.hash.IsNull()) {
        dexOffer.Create(myOffer);
    } else {
        dexOffer = CDexOffer(myOffer);
    }

    CDex dex(dexOffer);

    std::string error;
    uint256 tx;
    if (dex.PayForOffer(tx, error)) {
        dexman.sendOffer(dex.offer);

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

void TableOffersEditor::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    if (table == MyOffers && (operation == Add || operation == Edit) && status == Ok) {
        updateData();
        Q_EMIT dataChanged();
    }
}

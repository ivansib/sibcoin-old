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
    tableView->setColumnWidth(3, 150);
    tableView->setColumnWidth(4, 150);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(tableView, 150, 150, 2);

    updateData();

    connect(editor, &OfferDetailsEditor::dataSave, this, &TableOffersEditor::addOrEditDraftMyOffer);
    connect(editor, &OfferDetailsEditor::dataSend, this, &TableOffersEditor::sendMyOffer);
    connect(editor, &OfferDetailsEditor::draftDataDelete, this, &TableOffersEditor::deleteDraftData);

    connect(creator, &OfferDetailsCreator::dataSave, this, &TableOffersEditor::addOrEditDraftMyOffer);
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

void TableOffersEditor::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    columnResizingFixer->stretchColumnWidth(2);
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

void TableOffersEditor::addOrEditDraftMyOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo offer = ConvertData::fromQtMyOfferInfo(info);
    dexman.addOrEditDraftMyOffer(offer);
}

void TableOffersEditor::sendMyOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo myOffer = ConvertData::fromQtMyOfferInfo(info);
    std::string error;
    dexman.prepareAndSendMyOffer(myOffer, error);

    if (!error.empty()) {
        QMessageBox::warning(this, tr("Warning"), tr(error.c_str()));
    }
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

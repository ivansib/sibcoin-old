#include <QMessageBox>
#include "tableofferseditor.h"
#include "convertdata.h"
#include "net.h"
#include "../dex/dex.h"
#include "../dex/dexmanager.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent)
    : TableOffersDialog(db, new OfferModelEditor(), 4, CommonSettingsForOffers::MyOffer, parent)
{
    editor = new OfferDetailsEditor(db, this);
    creator = new OfferDetailsCreator(db, this);

    updateData();

    connect(editor, SIGNAL(dataSave(QtMyOfferInfo)), this, SLOT(addOrEditDraftMyOffer(QtMyOfferInfo)));
    connect(editor, SIGNAL(dataSend(QtMyOfferInfo)), this, SLOT(sendMyOffer(QtMyOfferInfo)));
    connect(editor, SIGNAL(draftDataDelete(QtMyOfferInfo)), this, SLOT(deleteDraftData(QtMyOfferInfo)));

    connect(creator, SIGNAL(dataSave(QtMyOfferInfo)), this, SLOT(addOrEditDraftMyOffer(QtMyOfferInfo)));
    connect(creator, SIGNAL(dataSend(QtMyOfferInfo)), this, SLOT(sendMyOffer(QtMyOfferInfo)));

    connect(this, SIGNAL(navigationDataUpdate()), editor, SLOT(updateNavigationData()));
    connect(this, SIGNAL(navigationDataUpdate()), creator, SLOT(updateNavigationData()));

    useMyOfferMode(true);
    init();

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 150);
    tableView->setColumnWidth(3, 150);
    tableView->setColumnWidth(4, 150);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(tableView, 150, 150, 2);
    connect(pModel, SIGNAL(layoutChanged()), this, SLOT(updateTable()));
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
    QtMyOfferInfo info = pModel->offerInfo(index);
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

void TableOffersEditor::updateTable()
{
    columnResizingFixer->stretchColumnWidth(2);

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 150);
    tableView->setColumnWidth(3, 150);
    tableView->setColumnWidth(4, 150);
}

void TableOffersEditor::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    if (table == MyOffers && (operation == Add || operation == Edit || operation == Delete) && status == Ok) {
        updateData();
        Q_EMIT dataChanged();
    }
}

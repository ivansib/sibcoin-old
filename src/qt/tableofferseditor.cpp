#include <QMessageBox>
#include "tableofferseditor.h"
#include "convertdata.h"
#include "net.h"
#include "dex/dex.h"
#include "dex/dexmanager.h"
#include "dex/dexsync.h"

TableOffersEditor::TableOffersEditor(DexDB *db, QDialog *parent)
    : TableOffersDialog(db, new OfferModelEditor(), 4, CommonSettingsForOffers::MyOffer, parent)
{
    editor = new OfferDetailsEditor(db, this);
    creator = new OfferDetailsCreator(db, this);

    pDelegate = new TableOfferEditorDelegate();
    tableView->setItemDelegate(pDelegate);

    threadDexDB = new ControllerThreadDexDB();

    updateData();

    connect(threadDexDB, SIGNAL(errorPrepareAndSendMyOffer(QString)), this, SLOT(showMessageWarning(QString)));

    connect(pDelegate, SIGNAL(clickedEdit(int)), this, SLOT(clickedButtonEdit(int)));
    connect(pDelegate, SIGNAL(clickedDelete(int)), this, SLOT(clickedButtonDelete(int)));

    connect(editor, SIGNAL(dataSave(QtMyOfferInfo)), this, SLOT(addOrEditDraftMyOffer(QtMyOfferInfo)));
    connect(editor, SIGNAL(dataSend(QtMyOfferInfo)), this, SLOT(sendMyOffer(QtMyOfferInfo)));
    connect(editor, SIGNAL(offerDelete(QtMyOfferInfo)), this, SLOT(deleteOffer(QtMyOfferInfo)));

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
    tableView->setColumnWidth(5, 100);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(tableView, 100, 150, 2);
    connect(pModel, SIGNAL(layoutChanged()), this, SLOT(resizeTable()));
}

TableOffersEditor::~TableOffersEditor()
{
    delete pDelegate;
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
    int limit = rowOnPage();
    int offset = limit * page;
    QList<QtMyOfferInfo> offers = ConvertData::toListQtMyOfferInfo(db->getMyOffers(countryIso, currencyIso, paymentMethod, offerType, 0, limit, offset));
    pModel->setOffers(offers);
    updatePageInfo();
}

void TableOffersEditor::saveMyOffer(const MyOfferInfo &info)
{
    if (db->isExistMyOfferByHash(info.hash)) {
        db->editMyOffer(info);
    } else {
        db->addMyOffer(info);
    }
}

int TableOffersEditor::countOffers() const
{
    return db->countMyOffers(countryIso, currencyIso, paymentMethod, offerType, 0);
}

void TableOffersEditor::clickedButtonEdit(const int &index)
{
    QtMyOfferInfo info = pModel->offerInfo(index);
    editor->setOfferInfo(info);
    editor->show();
}

void TableOffersEditor::clickedButtonDelete(const int &index)
{
    auto info = ConvertData::fromQtMyOfferInfo(pModel->offerInfo(index));
    if (dex::dexsync.isSynced()) {
        QMessageBox::StandardButton retval = editor->isDeleteOffer(info.status);

        if(retval == QMessageBox::Yes) {
            db->deleteMyOfferByHash(info.hash);
        }
    } else {
        editor->messageSyncDexNotFinished();
    }
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
    threadDexDB->prepareAndSendMyOffer(info);
}

void TableOffersEditor::deleteOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo myOffer = ConvertData::fromQtMyOfferInfo(info);

    db->deleteMyOfferByHash(myOffer.hash);
}

void TableOffersEditor::showMessageWarning(const QString &warning)
{
    QMessageBox::warning(this, tr("Warning"), warning);
}

void TableOffersEditor::resizeTable()
{
    columnResizingFixer->stretchColumnWidth(2);

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 150);
    tableView->setColumnWidth(3, 150);
    tableView->setColumnWidth(4, 150);
    tableView->setColumnWidth(5, 100);
}

void TableOffersEditor::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    if (table == MyOffers && (operation == Add || operation == Edit || operation == Delete) && status == Ok) {
        updateTable();
    }
}

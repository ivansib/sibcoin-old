#include "tableoffersview.h"
#include "convertdata.h"
#include "dex/dexsync.h"

TableOffersView::TableOffersView(DexDB *db, const TypeOffer &type, const CommonSettingsForOffers::TypeSettings &typeSettings, QDialog *parent)
    : TableOffersDialog(db, new OfferModelView, 3, typeSettings, parent), type(type)
{
    details = new OfferDetailsView(db, this);

    updateData();
    init();

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 150);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(tableView, 150, 150, 1);
    connect(pModel, SIGNAL(layoutChanged()), this, SLOT(resizeTable()));
}

TableOffersView::~TableOffersView()
{
}

void TableOffersView::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    columnResizingFixer->stretchColumnWidth(1);
}

int TableOffersView::countOffers() const
{
    if (type == Buy) {
        return db->countOffersBuy(countryIso, currencyIso, paymentMethod);
    }

    return db->countOffersSell(countryIso, currencyIso, paymentMethod);
}

void TableOffersView::updateData()
{
    int limit = rowOnPage();
    int offset = limit * page;

    QList<QtOfferInfo> offers;
    if (type == Buy) {
        offers = ConvertData::toListQtOfferInfo(db->getOffersBuy(countryIso, currencyIso, paymentMethod, limit, offset));
    } else {
        offers = ConvertData::toListQtOfferInfo(db->getOffersSell(countryIso, currencyIso, paymentMethod, limit, offset));
    }

    pModel->setOffers(offers);
    updatePageInfo();
}

void TableOffersView::clickedButton(const int &index)
{
    QtOfferInfo info = pModel->offerInfo(index);
    details->setOfferInfo(info);
    details->show();
}

void TableOffersView::updateTables(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    if (type == Buy) {
        if (table == OffersBuy && (operation == Add || operation == Edit || operation == Delete) && status == Ok) {
            updateTable();
        }
    } else if (type == Sell) {
        if (table == OffersSell && (operation == Add || operation == Edit || operation == Delete) && status == Ok) {
            updateTable();
        }
    }
}

void TableOffersView::resizeTable()
{
    columnResizingFixer->stretchColumnWidth(1);

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 150);
}

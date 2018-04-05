#include "tableoffersview.h"
#include "convertdata.h"

TableOffersView::TableOffersView(DexDB *db, const TypeOffer &type, const CommonSettingsForOffers::TypeSettings &typeSettings, QDialog *parent)
    : TableOffersDialog(db, new OfferModelView, 3, typeSettings, parent), type(type)
{
    details = new OfferDetailsView(db, this);

    updateData();

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 150);

    columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(tableView, 150, 150, 1);
    connect(pModel, SIGNAL(layoutChanged()), this, SLOT(updateTable()));
}

TableOffersView::~TableOffersView()
{
}

void TableOffersView::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    columnResizingFixer->stretchColumnWidth(1);
}

void TableOffersView::updateData()
{
    QList<QtOfferInfo> offers;
    if (type == Buy) {
        offers = ConvertData::toListQtOfferInfo(db->getOffersBuy());
    } else {
        offers = ConvertData::toListQtOfferInfo(db->getOffersSell());
    }

    pModel->setOffers(offers);

    init();
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
            updateData();
            Q_EMIT dataChanged();
        }
    } else if (type == Sell) {
        if (table == OffersSell && (operation == Add || operation == Edit || operation == Delete) && status == Ok) {
            updateData();
            Q_EMIT dataChanged();
        }
    }
}

void TableOffersView::updateTable()
{
    columnResizingFixer->stretchColumnWidth(1);

    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 150);
}

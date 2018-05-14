#include "tableofferviewdelegate.h"

TableOfferViewDelegate::TableOfferViewDelegate(QObject *parent) : TableOfferDelegate(parent), columnBtn(3)
{

}

void TableOfferViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == columnBtn) {
        buttonStyle(painter, option, index);
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget *TableOfferViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == columnBtn) {
        Button *btn = new Button(index.data(Qt::EditRole).toString(), index.row(), parent);
        connect(btn, SIGNAL(clicked(int)), this, SIGNAL(clicked(int)));
        return btn;
    }

    return QItemDelegate::createEditor(parent, option, index);
}

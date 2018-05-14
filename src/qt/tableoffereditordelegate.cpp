#include "tableoffereditordelegate.h"

TableOfferEditorDelegate::TableOfferEditorDelegate(QObject *parent) : TableOfferDelegate(parent), columnBtnEdit(4), columnBtnDelete(5)
{

}

void TableOfferEditorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == columnBtnEdit || index.column() == columnBtnDelete) {
        buttonStyle(painter, option, index);
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget *TableOfferEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == columnBtnEdit || index.column() == columnBtnDelete) {
        Button *btn = new Button(index.data(Qt::EditRole).toString(), index.row(), parent);

        if (index.column() == columnBtnEdit) {
            connect(btn, SIGNAL(clicked(int)), this, SIGNAL(clickedEdit(int)));
        } else {
            connect(btn, SIGNAL(clicked(int)), this, SIGNAL(clickedDelete(int)));
        }

        return btn;
    }

    return QItemDelegate::createEditor(parent, option, index);
}

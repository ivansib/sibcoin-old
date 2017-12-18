#include <QPushButton>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include "tableofferdelegate.h"

TableOfferDelegate::TableOfferDelegate(QObject *parent) : QItemDelegate(parent)
{

}

void TableOfferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 3) {
        QStyleOptionButton button;
        button.rect = option.rect;
        button.text = index.data(Qt::EditRole).toString();
        button.state = QStyle::State_Enabled;

        QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget *TableOfferDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == 3) {
        QPushButton *btn = new QPushButton(index.data(Qt::EditRole).toString(), parent);
        connect(btn, &QPushButton::clicked, [=]{clicked(index.row());});
        return btn;
    }

    return QItemDelegate::createEditor(parent, option, index);
}

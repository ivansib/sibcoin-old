#include <QPushButton>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include "tableofferdelegate.h"

TableOfferDelegate::TableOfferDelegate(const int &columnBtn, QObject *parent) : QItemDelegate(parent), columnBtn(columnBtn)
{
}

void TableOfferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == columnBtn) {
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
    if (index.column() == columnBtn) {
        QPushButton *btn = new QPushButton(index.data(Qt::EditRole).toString(), parent);
        connect(btn, &QPushButton::clicked, [=]{clicked(index.row());});
        return btn;
    }

    return QItemDelegate::createEditor(parent, option, index);
}

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
        Button *btn = new Button(index.data(Qt::EditRole).toString(), index.row(), parent);
        connect(btn, SIGNAL(clicked(int)), this, SIGNAL(clicked(int)));
        return btn;
    }

    return QItemDelegate::createEditor(parent, option, index);
}

Button::Button(const QString &title, const int &index, QWidget *parent) : QPushButton(title, parent), index(index)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void Button::onClicked()
{
    Q_EMIT clicked(index);
}

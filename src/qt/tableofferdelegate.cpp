#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include "tableofferdelegate.h"

TableOfferDelegate::TableOfferDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void TableOfferDelegate::buttonStyle(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton button;
    button.rect = option.rect;
    button.text = index.data(Qt::EditRole).toString();
    button.state = QStyle::State_Enabled;

    QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);
}

Button::Button(const QString &title, const int &index, QWidget *parent) : QPushButton(title, parent), index(index)
{
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void Button::onClicked()
{
    Q_EMIT clicked(index);
}

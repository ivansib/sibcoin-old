#include <QApplication>
#include <QPainter>
#include "settingsdelegate.h"

SettingsDelegate::SettingsDelegate(const int &columnEdit, QObject *parent) : QItemDelegate(parent), columnEdit(columnEdit)
{

}

void SettingsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == columnEdit) {
        bool value = index.data(Qt::EditRole).toBool();

        QStyleOptionButton checkboxIndicator;

        checkboxIndicator.state |= QStyle::State_Enabled;
        checkboxIndicator.state |= (value) ? QStyle::State_On : QStyle::State_Off;

        checkboxIndicator.rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxIndicator, NULL);

        const int x = option.rect.center().x() - checkboxIndicator.rect.width() / 2;
        const int y = option.rect.center().y() - checkboxIndicator.rect.height() / 2;

        checkboxIndicator.rect.moveTo( x, y );

        QColor colorBackground = option.palette.highlight().color();
        colorBackground.setAlpha(130);

        if ((option.state & QStyle::State_Enabled) && !(option.state & QStyle::State_Selected)) {
            QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxIndicator, painter );
        } else if (option.state & QStyle::State_Active) {
            QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxIndicator, painter );
            painter->fillRect(option.rect, QBrush(colorBackground));
        }

        if ((option.state & QStyle::State_Enabled) && (option.state & QStyle::State_HasFocus) && (option.state & QStyle::State_Active)
                && (option.state & QStyle::State_Selected)) {
            painter->fillRect(option.rect, QBrush(colorBackground));
            QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxIndicator, painter);
        } else if ((option.state & QStyle::State_Selected) && (option.state & QStyle::State_MouseOver && !(option.state & QStyle::State_Active))) {
            painter->fillRect(option.rect, QBrush(QColor(0, 0, 255, 130)));
        }
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget *SettingsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == columnEdit) {
        BooleanWidget *w = new BooleanWidget(parent);
        return w;
    }

    return QItemDelegate::createEditor(parent, option, index);
}

void SettingsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == columnEdit) {
        bool enabled = index.data().toBool();
        BooleanWidget *w = qobject_cast<BooleanWidget *>(editor);
        w->setChecked(enabled);
    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

void SettingsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == columnEdit) {
        BooleanWidget *w = qobject_cast<BooleanWidget *>(editor);
        model->setData(index, QVariant::fromValue(w->isChecked()));
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

QSize SettingsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(option, index);
}

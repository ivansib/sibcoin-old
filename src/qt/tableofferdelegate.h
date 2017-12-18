#ifndef TABLEOFFERDELEGATE_H
#define TABLEOFFERDELEGATE_H

#include <QItemDelegate>

class TableOfferDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    TableOfferDelegate(QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

Q_SIGNALS:
    void clicked(const int &) const;
};

#endif

#ifndef TABLEOFFERVIEWDELEGATE_H
#define TABLEOFFERVIEWDELEGATE_H

#include "tableofferdelegate.h"

class TableOfferViewDelegate : public TableOfferDelegate
{
    Q_OBJECT

public:
    TableOfferViewDelegate(QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const int columnBtn;

Q_SIGNALS:
    void clicked(const int &) const;
};

#endif // TABLEOFFERVIEWDELEGATE_H

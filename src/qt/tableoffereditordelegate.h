#ifndef TABLEOFFEREDITORDELEGATE_H
#define TABLEOFFEREDITORDELEGATE_H

#include "tableofferdelegate.h"

class TableOfferEditorDelegate : public TableOfferDelegate
{
    Q_OBJECT

public:
    TableOfferEditorDelegate(QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const int columnBtnEdit;
    const int columnBtnDelete;

Q_SIGNALS:
    void clickedEdit(const int &) const;
    void clickedDelete(const int &) const;
};

#endif // TABLEOFFEREDITORDELEGATE_H

#ifndef TABLEOFFERDELEGATE_H
#define TABLEOFFERDELEGATE_H

#include <QItemDelegate>
#include <QPushButton>

class TableOfferDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    TableOfferDelegate(const int &columnBtn, QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    int columnBtn;

Q_SIGNALS:
    void clicked(const int &) const;
};

class Button : public QPushButton
{
    Q_OBJECT

public:
    Button(const QString &title, const int &index, QWidget *parent);

private:
    const int index;

private Q_SLOTS:
    void onClicked();

Q_SIGNALS:
    void clicked(const int &index);
};

#endif

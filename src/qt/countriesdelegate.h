#ifndef COUNTRIESDELEGATE_H
#define COUNTRIESDELEGATE_H

#include <QItemDelegate>
#include <QHBoxLayout>
#include <QCheckBox>

class BooleanWidget : public QWidget
{
    Q_OBJECT
    QCheckBox * checkBox;

public:
    BooleanWidget(QWidget * parent = nullptr) : QWidget(parent) {
        checkBox = new QCheckBox(this);
        QHBoxLayout * layout = new QHBoxLayout(this);
        layout->addWidget(checkBox,0, Qt::AlignCenter);
    }

    bool isChecked() {
        return checkBox->isChecked();
    }

    void setChecked(bool value) {
        checkBox->setChecked(value);
    }
};

class CountriesDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    CountriesDelegate(const int &columnEdit, QObject *parent = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const int columnEdit;
};

#endif

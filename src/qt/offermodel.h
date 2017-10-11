#ifndef OFFERMODEL_H
#define OFFERMODEL_H

#include <QAbstractTableModel>
#include "dex/dexdto.h"
#include <list>

using namespace dex;

class OfferModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OfferModel(const std::list<OfferInfo> &offers, QObject *parent = nullptr);
    ~OfferModel();

    virtual QVariant data(const QModelIndex &index, int role) const;

    virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QStringList listHead;
    QList<OfferInfo> offers;
};

#endif

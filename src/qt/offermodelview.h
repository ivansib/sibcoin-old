#ifndef OFFERMODELVIEW_H
#define OFFERMODELVIEW_H

#include "offermodel.h"

class OfferModelView : public OfferModel<QtOfferInfo> {
public:
    OfferModelView(QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

protected:
    virtual void filterOffers();
};

#endif

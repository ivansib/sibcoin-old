#ifndef OFFERDETAILSEDITOR_H
#define OFFERDETAILSEDITOR_H

#include "offerdetails.h"

class OfferDetailsEditor : public OfferDetails
{
    Q_OBJECT

public:
    OfferDetailsEditor(DexDB *db, QDialog *parent = nullptr);

    void setOfferInfo(const QtMyOfferInfo &info);

protected Q_SLOTS:
    virtual void saveData();
};

#endif

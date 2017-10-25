#ifndef OFFERDETAILSCREATOR_H
#define OFFERDETAILSCREATOR_H

#include "offerdetails.h"

class OfferDetailsCreator : public OfferDetails
{
    Q_OBJECT

public:
    OfferDetailsCreator(DexDB *db, QDialog *parent = nullptr);

protected Q_SLOTS:
    virtual void saveData();
};

#endif

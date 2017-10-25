#include "offerdetailscreator.h"

OfferDetailsCreator::OfferDetailsCreator(DexDB *db, QDialog *parent) : OfferDetails(db, OfferDetails::Create, parent)
{
}

void OfferDetailsCreator::saveData()
{
    close();
}

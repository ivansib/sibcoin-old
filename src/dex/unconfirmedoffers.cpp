#include "unconfirmedoffers.h"

UnconfirmedOffers::UnconfirmedOffers()
{

}

void UnconfirmedOffers::getOffer(const CDexOffer &offer)
{
    offers.push_back({offer, time(NULL)});
}

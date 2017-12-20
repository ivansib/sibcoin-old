#ifndef UNCONFIRMEDOFFERS_H
#define UNCONFIRMEDOFFERS_H

#include <ctime>
#include "dexoffer.h"

class UnconfirmedOffers {
public:
    UnconfirmedOffers();

    void getOffer(const CDexOffer &offer);

private:
    struct OfferTime {
        CDexOffer offer;
        std::time_t timeArr;
    };

    std::list<OfferTime> offers;
};

#endif

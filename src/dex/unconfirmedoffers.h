#ifndef UNCONFIRMEDOFFERS_H
#define UNCONFIRMEDOFFERS_H

#include <ctime>
#include "dexoffer.h"

class UnconfirmedOffers {
public:
    UnconfirmedOffers();

    void setOffer(const CDexOffer &offer);
    std::list<uint256> hashs() const;
    bool isExistOffer(const uint256 &hash);
    CDexOffer getOffer(const uint256 &hash) const;

private:
    struct OfferTime {
        CDexOffer offer;
        std::time_t timeArr;
    };

    std::list<OfferTime> offers;
};

#endif

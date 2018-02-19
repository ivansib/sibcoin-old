#include "unconfirmedoffers.h"

UnconfirmedOffers::UnconfirmedOffers()
{

}

void UnconfirmedOffers::setOffer(const CDexOffer &offer)
{
    offers.push_back({offer, time(NULL)});
}

std::list<uint256> UnconfirmedOffers::hashs() const
{
    std::list<uint256> h;

    for (auto item : offers) {
        h.push_back(item.offer.hash);
    }

    return h;
}

bool UnconfirmedOffers::isExistOffer(const uint256 &hash) const
{
    for (auto item : offers) {
        if (item.offer.hash == hash) {
            return true;
        }
    }

    return false;
}

CDexOffer UnconfirmedOffers::getOffer(const uint256 &hash) const
{
    for (auto item : offers) {
        if (item.offer.hash == hash) {
            return item.offer;
        }
    }

    return CDexOffer();
}

std::list<CDexOffer> UnconfirmedOffers::getOffers() const
{
    std::list<CDexOffer> off;

    for (auto item : offers) {
        off.push_back(item.offer);
    }

    return off;
}

void UnconfirmedOffers::deleteOldOffers()
{
    std::time_t currentTime = time(NULL);

    offers.remove_if([currentTime](OfferTime o){ return difftime(o.timeArr, currentTime) >= 21600; }); // 6 hours
}

void UnconfirmedOffers::deleteOffer(const uint256 &hash)
{
    offers.remove_if([hash](OfferTime o){ return o.offer.hash == hash; });
}

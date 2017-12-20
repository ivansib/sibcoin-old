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

bool UnconfirmedOffers::isExistOffer(const uint256 &hash)
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

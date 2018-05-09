#ifndef UNCONFIRMEDOFFERS_H
#define UNCONFIRMEDOFFERS_H

#include <ctime>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include "dexoffer.h"

namespace dex {

struct UnconfirmedOffersComparator {

    bool operator()(const CDexOffer& a, const CDexOffer& b) const {
        return a.timeCreate < b.timeCreate || a.hash < b.hash || a.editingVersion < b.editingVersion;
    }

}

class UnconfirmedOffers {
public:
    UnconfirmedOffers();

    void setOffer(const CDexOffer &offer);
    std::list<uint256> hashs();
    bool isExistOffer(const uint256 &hash); 
    CDexOffer getOffer(const uint256 &hash);
    std::list<CDexOffer> getOffers();
    void deleteOldOffers();
    void deleteOffer(const uint256 &hash);

private:
    struct OfferTime {
        CDexOffer offer;
        std::time_t timeArr;
    };

    std::map<CDexOffer,std::time_t,dex::UnconfirmedOffersComparator> offers;
    boost::shared_mutex smOfferMutex;
}

#endif

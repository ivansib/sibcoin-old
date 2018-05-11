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

};

class UnconfirmedOffers {
public:
    UnconfirmedOffers();

    bool putOffer(const CDexOffer &offer);
    CDexOffer getOfferByHash(const uint256 &hash);

    bool updateOffer(const CDexOffer &offer);
    
    bool hasOffer(const CDexOffer &offer);
    bool hasOfferWithHash(const uint256 &hash); 

    std::list<CDexOffer> getAllOffers();

    bool removeOffer(const CDexOffer &offer);    
    
    void deleteOldOffers();
    
    size_t getSize();
    std::list<CDexOffer> getLastOffers(size_t number);
    std::list<CDexOffer> getOffersPriorTo(const CDexOffer &offer);

private:
    
    CDexOffer findByHash(const uint256 &hash);
    
    std::map<CDexOffer,std::time_t,dex::UnconfirmedOffersComparator> offers;

    boost::mutex mOfferMutex;
};

}

#endif

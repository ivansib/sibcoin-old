#include "unconfirmedoffers.h"

namespace dex {

UnconfirmedOffers::UnconfirmedOffers()
{

}

/**
 * 
 * @param CDexOffer
 * @return bool true if entry was inserted and false otherwise
 */
bool UnconfirmedOffers::putOffer(const CDexOffer &offer)
{
    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    auto result = offers.insert(std::make_pair(offer, std::time(NULL)));
    return result.second;
}


bool UnconfirmedOffers::hasOfferWithHash(const uint256 &hash)
{
    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    CDexOffer offer = findByHash(hash);

    return !offer.IsNull();
}


bool UnconfirmedOffers::hasOffer(const CDexOffer &offer)
{
    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    
    if ( offers.find(offer) != offers.end() ) {
        return true;
    }

    return false;
}



CDexOffer UnconfirmedOffers::getOfferByHash(const uint256 &hash)
{
    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    return findByHash(hash);
}

std::list<CDexOffer> UnconfirmedOffers::getAllOffers()
{
    std::list<CDexOffer> off;

    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    for (std::map<CDexOffer,std::time_t>::const_iterator it = offers.begin(); it != offers.end(); ++it)
    {
        CDexOffer offer = it->first;
        off.push_back(offer);
    }

    return off;
}

void UnconfirmedOffers::deleteOldOffers()
{
    std::time_t currentTime = time(NULL);
    boost::lock_guard<boost::mutex> lock(mOfferMutex);

    std::map<CDexOffer,std::time_t>::const_iterator it = offers.begin();
    for (; it != offers.end();)
    {
        if(difftime(currentTime, it->second) >= UNCONFIRMED_OFFER_LIFETIME)
            offers.erase(it++);
        else
            ++it;
    }
}

bool UnconfirmedOffers::removeOffer(const CDexOffer &offer)
{
    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    return offers.erase(offer) > 0;
}

void UnconfirmedOffers::removeOffers(std::vector<CDexOffer> &voffers)
{
    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    for (auto offer : voffers) {
        offers.erase(offer);
    }
}



size_t UnconfirmedOffers::getSize() 
{
    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    return offers.size();
}


std::list<CDexOffer> UnconfirmedOffers::getLastOffers(size_t number) 
{
    std::list<CDexOffer> off;

    boost::lock_guard<boost::mutex> lock(mOfferMutex);
    for (std::map<CDexOffer,std::time_t>::const_reverse_iterator it = offers.crbegin(); it != offers.crend(); ++it)
    {
        CDexOffer offer = it->first;
        off.push_back(offer);
    }

    return off;

}


/**
 * 
 * @param CDexOffer offer
 * @return bool true if offer was updated
 */
bool UnconfirmedOffers::updateOffer(const CDexOffer& offer) {

    boost::lock_guard<boost::mutex> lock(mOfferMutex);

    // we already have it, no update needed
    if ( offers.find(offer) != offers.end() ) {
        return false;
    }    
    
    CDexOffer foundOffer = findByHash(offer.hash);
    
    // previous version found
    if (!foundOffer.IsNull() && (offer.editingVersion > foundOffer.editingVersion) ) {
        offers.erase(foundOffer);
    } 
    
    offers.insert(std::make_pair(offer, std::time(NULL)));
    return true;
}


CDexOffer UnconfirmedOffers::findByHash(const uint256& hash) 
{
    for (std::map<CDexOffer,std::time_t>::const_iterator it = offers.begin(); it != offers.end(); ++it)
    {
        CDexOffer offer = it->first;
        if(offer.hash == hash)
            return offer;
    }
    
    return CDexOffer();
}



}

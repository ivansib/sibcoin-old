#include "unconfirmedoffers.h"

UnconfirmedOffers::UnconfirmedOffers()
{

}

void UnconfirmedOffers::setOffer(const CDexOffer &offer)
{
    boost::unique_lock<boost::shared_mutex> lock(smOfferMutex);
    offers.insert(std::make_pair(offer, std::time(NULL)));
}

std::list<uint256> UnconfirmedOffers::hashs()
{
    std::list<uint256> h;

    boost::shared_lock<boost::shared_mutex> lock(smOfferMutex);
    for (std::map<CDexOffer, std::time_t>::const_iterator it = offers.begin(); it != offers.end(); ++it)
    {
        CDexOffer offer = it->first;
        h.push_back(offer.hash);
    }

    return h;
}

bool UnconfirmedOffers::isExistOffer(const uint256 &hash)
{
    boost::shared_lock<boost::shared_mutex> lock(smOfferMutex);
    for (std::map<CDexOffer,std::time_t>::const_iterator it = offers.begin(); it != offers.end(); ++it)
    {
        CDexOffer offer = it->first;
        if(offer.hash == hash)
            return true;
    }

    return false;
}

CDexOffer UnconfirmedOffers::getOffer(const uint256 &hash)
{
    boost::shared_lock<boost::shared_mutex> lock(smOfferMutex);
    for (std::map<CDexOffer,std::time_t>::const_iterator it = offers.begin(); it != offers.end(); ++it)
    {
        CDexOffer offer = it->first;
        if(offer.hash == hash)
            return offer;
    }

    return CDexOffer();
}

std::list<CDexOffer> UnconfirmedOffers::getOffers()
{
    std::list<CDexOffer> off;

    boost::shared_lock<boost::shared_mutex> lock(smOfferMutex);
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
    boost::unique_lock<boost::shared_mutex> lock(smOfferMutex);

    std::map<CDexOffer,std::time_t>::const_iterator it = offers.begin();
    for (; it != offers.end();)
    {
        if(difftime(it->second, currentTime) >= 21600)
            offers.erase(it++);
        else
            ++it;
    }
}

void UnconfirmedOffers::deleteOffer(const uint256 &hash)
{
    boost::unique_lock<boost::shared_mutex> lock(smOfferMutex);

    std::map<CDexOffer,std::time_t>::const_iterator it = offers.begin();
    for (; it != offers.end();)
    {
        CDexOffer offer = it->first;
        if( offer.hash == hash )
            offers.erase(it++);    
        else
            ++it;
    }
}

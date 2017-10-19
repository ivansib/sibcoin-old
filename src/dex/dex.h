
#ifndef __DEX_H__
#define __DEX_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "timedata.h"
#include "dex/dexdto.h"

class CDexBroadcast;

class CDex
{
private:
    mutable CCriticalSection cs;
    dex::OfferInfo offer;

public:

    CDex();
    CDex(const CDex&);
    CDex(const CDexBroadcast&);


//??
//    Create(uint256 idTransaction, uint256 hash, std::string countryIso, std::string currencyIso,
//           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, uint64_t timeCreate,
//           uint64_t timeExpiration, std::string shortInfo, std::string details);



    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        LOCK(cs);
        READWRITE(offer.idTransaction);
        READWRITE(offer.hash);
        READWRITE(offer.countryIso);
        READWRITE(offer.currencyIso);
        READWRITE(offer.paymentMethod);
        READWRITE(offer.price);
        READWRITE(offer.minAmount);
        READWRITE(offer.timeCreate);
        READWRITE(offer.timeExpiration);
        READWRITE(offer.shortInfo);
        READWRITE(offer.details);
    }

//    void Check(bool fForce = false);

};



#endif // __DEX_H__
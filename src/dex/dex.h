
#ifndef __DEX_H__
#define __DEX_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "timedata.h"
#include "dex/dexdto.h"
#include "dexoffer.h"


class CDex
{
private:
    mutable CCriticalSection cs;

public:
    CDexOffer offer;

public:

    CDex();
    CDex(const CDexOffer &dexoffer);

    bool CreateOffer(CDexOffer::Type type, const uint256 &idTransaction, const std::string &countryIso,
                     const std::string &currencyIso, uint8_t paymentMethod, uint64_t price,
                     uint64_t minAmount, int timeExpiration, const std::string &shortInfo, const std::string &details);

    bool CreateOffer(CDexOffer::Type type, const std::string &countryIso,
                     const std::string &currencyIso, uint8_t paymentMethod, uint64_t price,
                     uint64_t minAmount, int timeExpiration, const std::string &shortInfo, const std::string &details);

    bool addOfferToDB();

    // оплата предложения (создание, подпись и ретрансляция транзакции)
    // хеш транзакции помещается в offer
    bool PayForOffer(CTransaction &newTx, std::string &sError);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        LOCK(cs);
        READWRITE(offer);
    }


};


#endif // __DEX_H__
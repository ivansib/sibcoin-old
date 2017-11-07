
#ifndef __DEX_OFFER_H__
#define __DEX_OFFER_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "timedata.h"
#include "dex/dexdto.h"



static const int DEX_SHORT_INFO_LENGTH     =  140;
static const int DEX_DETAILS_LENGTH        = 1024;


class CDexOffer
{
private:
    mutable CCriticalSection cs;

public:
   enum Type { BUY, SELL };

    uint256 hash;
    uint256 idTransaction;
    std::string type;
    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;
    uint64_t price;
    uint64_t minAmount;
    uint64_t timeCreate;
    int timeExpiration;
    std::string shortInfo;
    std::string details;


public:

    CDexOffer();

    void SetNull();

    bool isNull() const
    {
      return (hash.IsNull());
    }


    bool Create(Type type, const std::string &countryIso, const std::string &currencyIso,
           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, int timeExpiration,
           const std::string &shortInfo, const std::string &details);

    bool Create(const uint256 &idTransaction, Type type, const std::string &countryIso, const std::string &currencyIso,
           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, int timeExpiration,
           const std::string &shortInfo, const std::string &details);


    operator dex::OfferInfo() const;
    CDexOffer& operator=(const CDexOffer&);

    std::string getType() const;
    bool isBuy() const;
    bool isSell() const;



    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        LOCK(cs);
        if (!(nType & SER_GETHASH)) {
            READWRITE(hash);
            READWRITE(idTransaction);
        }
        READWRITE(type);
        READWRITE(countryIso);
        READWRITE(currencyIso);
        READWRITE(paymentMethod);
        READWRITE(price);
        READWRITE(minAmount);
        READWRITE(timeCreate);
        READWRITE(timeExpiration);
        READWRITE(shortInfo);
        READWRITE(details);
    }

    uint256 MakeHash();

    std::string dump() const;

    bool Check(bool fullcheck);

};



#endif // __DEX_OFFER_H__

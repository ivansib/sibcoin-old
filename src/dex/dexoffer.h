
#ifndef __DEX_OFFER_H__
#define __DEX_OFFER_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "utilstrencodings.h"
#include "timedata.h"
#include "dex/db/dexdto.h"
#include <univalue.h>

namespace dex {

class CDexOffer
{
private:
    mutable CCriticalSection cs;

public:
   enum Type { BUY, SELL };

    uint256 hash;
    uint256 idTransaction;
    std::string pubKey;
    std::string type;
    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;
    uint64_t price;
    uint64_t minAmount;
    uint64_t timeCreate;
    uint64_t timeExpiration;
    uint64_t timeModification;
    std::string shortInfo;
    std::string details;
    int editingVersion;
    std::string editsign;

    bool myoffer_;
    dex::StatusOffer status;

public:

    CDexOffer();
    CDexOffer(const CDexOffer &off);
    CDexOffer(const dex::OfferInfo &info, dex::TypeOffer offertype);
    CDexOffer(const dex::MyOfferInfo &info);

    void SetNull();

    bool IsNull() const
    {
      return (hash.IsNull());
    }

    bool Create(Type type, const std::string &pubKey, const std::string &countryIso, const std::string &currencyIso,
           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, time_t timeExpiration,
           const std::string &shortInfo, const std::string &details, const int &editingVersion = 1);

    bool Create(const uint256 &idTransaction, Type type, const std::string &pubKey, const std::string &countryIso, const std::string &currencyIso,
           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, time_t timeExpiration,
           const std::string &shortInfo, const std::string &details, const int &editingVersion = 1);

    bool Create(const dex::OfferInfo &info, dex::TypeOffer offertype);

    bool Create(const dex::MyOfferInfo &info);

    operator dex::OfferInfo() const;
    operator dex::MyOfferInfo() const;
    CDexOffer& operator=(const CDexOffer&);

    std::string getType() const;
    dex::TypeOffer getTypeOffer() const;
    bool isBuy() const;
    bool isSell() const;
    bool isMyOffer() const;

    CPubKey getPubKeyObject() const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        LOCK(cs);
        if (!(nType & SER_GETHASH)) {
            READWRITE(hash);
            READWRITE(idTransaction);
        }
        if (ser_action.ForRead()) {
            std::vector<unsigned char> vch;
            READWRITE(vch);
            pubKey = HexStr(vch);
        } else {
            std::vector<unsigned char> vch = ParseHex(pubKey);
            READWRITE(vch);
        }
        READWRITE(type);
        READWRITE(countryIso);
        READWRITE(currencyIso);
        READWRITE(paymentMethod);

        if (!(nType & SER_GETHASH)) {
            READWRITE(price);
        }

        READWRITE(minAmount);
        READWRITE(timeCreate);
        READWRITE(timeExpiration);

        if (!(nType & SER_GETHASH)) {
            READWRITE(shortInfo);
            READWRITE(details);
            READWRITE(editingVersion);
            READWRITE(timeModification);
        }
    }

    uint256 MakeHash();
    uint256 MakeEditHash();
    bool CheckEditSign();

    std::string dump() const;

    bool Check(bool fullcheck);
    bool Check(bool fullcheck, int &fine);
    bool CheckModTimeOnEdit();

    UniValue getUniValue();

};

}

#endif // __DEX_OFFER_H__

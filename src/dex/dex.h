
#ifndef __DEX_H__
#define __DEX_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "timedata.h"
#include "dex/db/dexdto.h"
#include "dexoffer.h"


#define PAYOFFER_MIN_TX_HEIGHT    1
#define PAYOFFER_RETURN_FEE       10000
#define PAYOFFER_TX_FEE           10000000
#define MAX_DEX_TRANSACTION_FEE   PAYOFFER_TX_FEE*3
#define MAX_TRANSACTION_SIZE      10000

namespace dex {

class CDex
{
private:
    mutable CCriticalSection cs;
    CTransaction payTx;
    
public:
    CDexOffer offer;

public:

    CDex();
    CDex(const CDexOffer &dexoffer);

    bool CreateOffer(CDexOffer::Type type, const uint256 &idTransaction, const std::string &pubKey, const std::string &countryIso,
                     const std::string &currencyIso, uint8_t paymentMethod, uint64_t price,
                     uint64_t minAmount, time_t timeExpiration, const std::string &shortInfo, const std::string &details, const uint32_t &editingVersion);

    bool CreateOffer(CDexOffer::Type type, const std::string &pubKey, const std::string &countryIso,
                     const std::string &currencyIso, uint8_t paymentMethod, uint64_t price,
                     uint64_t minAmount, time_t timeExpiration, const std::string &shortInfo, const std::string &details, const uint32_t &editingVersion);

    bool CreateOffer(const dex::MyOfferInfo &info);

    // оплата предложения (создание, подпись и ретрансляция транзакции)
    // хеш транзакции помещается в offer
    bool PayForOffer(uint256 &txid, std::string &sError);


    // проверка транзакции 
    bool CheckOfferTx(std::string &sError);

    // проверка подписи оффера публичным ключем
    bool CheckOfferSign(const std::vector<unsigned char> &vchSign, std::string &sError);

    // подписать оффер приватным ключем
    bool SignOffer(const CKey &key, std::vector<unsigned char> &vchSign, std::string &sError);

    // ищет в кошельке приватный ключ по публичному
    bool FindKey(CKey &key, std::string &sError);

    // проверка подписи редактирования оффера (поле editsign)
    bool CheckEditSign();

    // подписать изменения при редактировании приватным ключем (поле editsign)
    bool MakeEditSign(const CKey &key, std::string &sError);

    // транзакция на оплату оффера
    const CTransaction & getPayTx() const;

    // проверка транзакции на оплату в бродкасте
    bool CheckBRCSTOfferTx(const CTransaction &tx, std::string &sError);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        LOCK(cs);
        READWRITE(offer);
    }

private:

    bool CheckTx(const CTransaction &tx, std::string &sError);


};

}

#endif // __DEX_H__

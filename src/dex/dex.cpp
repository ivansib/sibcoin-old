
#include "dex.h"
#include "init.h"
#include "util.h"

#include "dexoffer.h"
#include "primitives/transaction.h"
#include "dex/dexdb.h"
#include "dextransaction.h"


#define CHECK(A,B,...) { if (!(A)) { std::string str = strprintf(std::string("%s\n") + (B), "",  ##__VA_ARGS__); LogPrintf("%s", str.c_str()); sError += str; break; } }


CDex::CDex()
{
}


CDex::CDex(const CDexOffer &dexoffer)
{
    offer = dexoffer;
}


bool CDex::CreateOffer(CDexOffer::Type type, const uint256 &idTransaction, const std::string &countryIso, const std::string &currencyIso,
           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, int timeExpiration,
           const std::string &shortInfo, const std::string &details)
{
    return offer.Create(idTransaction, type, countryIso, currencyIso, paymentMethod, price, minAmount, timeExpiration, shortInfo, details);
}


bool CDex::CreateOffer(CDexOffer::Type type, const std::string &countryIso, const std::string &currencyIso,
                        uint8_t paymentMethod, uint64_t price, uint64_t minAmount, int timeExpiration,
                        const std::string &shortInfo, const std::string &details)
{
    return offer.Create(type, countryIso, currencyIso, paymentMethod, price, minAmount, timeExpiration, shortInfo, details);
}



bool CDex::addOfferToDB()
{
    if (offer.isNull()) return false;

    dex::DexDB db(strDexDbFile);
    if (offer.isBuy())  db.addOfferBuy(offer);
    if (offer.isSell()) db.addOfferSell(offer);
    return true;
}


bool CDex::PayForOffer(uint256 &txid, std::string &sError)
{
    do {
        CHECK(!offer.isNull(), "Offer is empty");
        CHECK(offer.idTransaction.IsNull(), "The offer has already been paid");

        CHECK(CreatePayOfferTransaction(offer, payTx, sError), "");
        offer.idTransaction = payTx.GetHash();
        txid = offer.idTransaction;
        return true;
    } while(false);
    return false;
}




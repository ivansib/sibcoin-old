
#include "dex.h"
#include "init.h"
#include "util.h"

#include "dexoffer.h"
#include "dex/dexdb.h"


CDex::CDex()
{
}


bool CDex::CreateOffer(CDexOffer::Type type, const uint256 &idTransaction, const std::string &countryIso, const std::string &currencyIso,
           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, int timeExpiration,
           const std::string &shortInfo, const std::string &details)
{

    if (offer.Create(idTransaction, type, countryIso, currencyIso, 1,
                     price, minAmount, timeExpiration, shortInfo, details))
    {
        dex::DexDB db(strDexDbFile);
        if (type == CDexOffer::BUY)  db.addOfferBuy(offer);
        if (type == CDexOffer::SELL) db.addOfferSell(offer);
        return true;
    }
    return false;
}





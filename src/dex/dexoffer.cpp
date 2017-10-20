
#include "dexoffer.h"
#include "init.h"
#include "util.h"
#include "utiltime.h"


const char * OFFER_TYPE_BUY  =  "buy";
const char * OFFER_TYPE_SELL = "sell";




CDexOffer::CDexOffer()
{
  SetNull();
}



void CDexOffer::SetNull()
{
    hash.SetNull();
    idTransaction.SetNull();
    type.clear();
    countryIso.clear();
    currencyIso.clear();
    paymentMethod = 0;;
    price = 0;
    minAmount = 0;
    timeCreate = 0;
    timeExpiration = 0;
    shortInfo.clear();
    details.clear();
}





bool CDexOffer::Create(const uint256 &idTransaction_, Type type_, const std::string &countryIso_, const std::string &currencyIso_,
           uint8_t paymentMethod_, uint64_t price_, uint64_t minAmount_, uint64_t timeExpiration_,
           const std::string &shortInfo_, const std::string &details_)
{
    do {
        if (idTransaction_.IsNull()) break;
        if (countryIso.size() != 2) break;
        if (currencyIso.size() != 3) break;
        if (shortInfo_.size() > DEX_SHORT_INFO_LENGTH) break;
        if (details_.size() > DEX_DETAILS_LENGTH) break;
        //TODO: check timeExpiration > getTime() ??

        idTransaction   = idTransaction_;
        paymentMethod   = paymentMethod_; 
        price           = price_;
        minAmount       = minAmount_;
        timeCreate      = GetTime();
        timeExpiration  = timeExpiration_;
        shortInfo       = shortInfo_;
        details         = details_;

        switch (type_) {
            case  BUY: type = OFFER_TYPE_BUY;  break;
            case SELL: type = OFFER_TYPE_SELL; break;
        }
        hash = MakeHash();

        LogPrintf("Create DexOffer\n\tidtransaction\t%s\nhash\t\t\t%s\n", idTransaction.GetHex().c_str(), hash.GetHex().c_str());

        return true;
    } while (false);
    SetNull();
    return false;
}



uint256 CDexOffer::MakeHash()
{
    return SerializeHash(*this);
}




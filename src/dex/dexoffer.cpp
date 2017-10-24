
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
           uint8_t paymentMethod_, uint64_t price_, uint64_t minAmount_, int timeExpiration_,
           const std::string &shortInfo_, const std::string &details_)
{
    do {
        if (idTransaction_.IsNull()) {
            LogPrintf("Create DexOffer --  idTransaction is NULL\n");
            break;
        }
        if (countryIso_.size() != 2) {
            LogPrintf("Create DexOffer --  countryIso is NULL\n");
            break;
        }
        if (currencyIso_.size() != 3) {
            LogPrintf("Create DexOffer --  currencyIso is NULL\n");
            break;
        }
        if (shortInfo_.size() > DEX_SHORT_INFO_LENGTH) {
            LogPrintf("Create DexOffer --  shortinfo string too long\n");
            break;
        }
        if (details_.size() > DEX_DETAILS_LENGTH) {
            LogPrintf("Create DexOffer --  details string too long\n");
            break;
        }

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
        LogPrintf("Create DexOffer\n%s\n", dump().c_str());
        return true;
    } while (false);
    SetNull();
    return false;
}



uint256 CDexOffer::MakeHash()
{
    return SerializeHash(*this);
}



CDexOffer::operator dex::OfferInfo() const
{
    dex::OfferInfo info;
    info.idTransaction    = idTransaction;
    info.hash             = hash;
    info.countryIso       = countryIso;
    info.currencyIso      = currencyIso;
    info.paymentMethod    = paymentMethod;
    info.price            = price;
    info.minAmount        = minAmount;
    info.timeCreate       = timeCreate;
    info.timeToExpiration = timeExpiration;
    info.shortInfo        = shortInfo;
    info.details          = details;
    return info;
}



std::string CDexOffer::getType() const
{
  return type;
}


bool CDexOffer::isBuy() const
{
  return type == OFFER_TYPE_BUY;
}


bool CDexOffer::isSell() const
{
  return type == OFFER_TYPE_SELL;
}



std::string CDexOffer::dump() const
{
   return strprintf("CDexOffer::dump()\n"
        "\ttype\t\t%s\n"
        "\tidTransaction\t%s\n"
        "\thash\t\t%s\n"
        "\tcountryIso\t%s\n"
        "\tcurrencyIso\t%s\n"
        "\tpaymentMethod\t%d\n"
        "\tprice\t\t%lld\n"
        "\tminAmount\t%lld\n"
        "\ttimeCreate\t%lld\n"
        "\ttimeExpiration\t%d\n"
        "\tshortInfo\t%s\n"
        "\tdetails\t\t%s\n",
        type.c_str(), idTransaction.GetHex().c_str(), hash.GetHex().c_str(), countryIso.c_str(),
        currencyIso.c_str(), paymentMethod, price, minAmount, timeCreate, timeExpiration,
        shortInfo.c_str(), details.c_str());
}




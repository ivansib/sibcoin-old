
#include "dexoffer.h"
#include "init.h"
#include "util.h"
#include "utiltime.h"
#include "db/countryiso.h"
#include "db/currencyiso.h"
#include "db/paymentmethodtype.h"


const char * OFFER_TYPE_BUY  =  "buy";
const char * OFFER_TYPE_SELL = "sell";


dex::CurrencyIso  defaultCurrencyIso;
dex::CountryIso  defaultCountryIso;
dex::PaymentMethodType defaultPaymentMethod;



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



bool CDexOffer::Create(Type type_, const std::string &countryIso_, const std::string &currencyIso_,
           uint8_t paymentMethod_, uint64_t price_, uint64_t minAmount_, int timeExpiration_,
           const std::string &shortInfo_, const std::string &details_)
{
    uint256 txid;
    return Create(txid, type_, countryIso_, currencyIso_, paymentMethod_, price_, minAmount_, timeExpiration_, shortInfo_, details_);
}



bool CDexOffer::Create(const uint256 &idTransaction_, Type type_, const std::string &countryIso_, const std::string &currencyIso_,
           uint8_t paymentMethod_, uint64_t price_, uint64_t minAmount_, int timeExpiration_,
           const std::string &shortInfo_, const std::string &details_)
{
    do {
        idTransaction   = idTransaction_;
        paymentMethod   = paymentMethod_;
        currencyIso     = currencyIso_;
        countryIso      = countryIso_;
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
        if (!Check(false)) break;
        hash = MakeHash();
        //LogPrintf("Create DexOffer\n%s\n", dump().c_str()); ///< for debug only
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

CDexOffer& CDexOffer::operator=(const CDexOffer& off)
{
    idTransaction    = off.idTransaction;
    hash             = off.hash;
    countryIso       = off.countryIso;
    currencyIso      = off.currencyIso;
    paymentMethod    = off.paymentMethod;
    price            = off.price;
    minAmount        = off.minAmount;
    timeCreate       = off.timeCreate;
    timeExpiration   = off.timeExpiration;
    shortInfo        = off.shortInfo;
    details          = off.details;
    return *this;
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


bool CDexOffer::Check(bool fullcheck)
{
    do {
        if (fullcheck && idTransaction.IsNull()) {
            LogPrintf("DexOffer::Check error: idTransaction is empty\n");
            break;
        }
        if (fullcheck && hash.IsNull()) {
            LogPrintf("DexOffer::Check(%s) error: hash is empty\n", idTransaction.GetHex().c_str());
            break;
        }
        if (fullcheck && hash != MakeHash()) {
            LogPrintf("DexOffer::Check(%s) error: hash not equal\n", idTransaction.GetHex().c_str());
            break;
        }
        if (countryIso.size() != 2) {
            LogPrintf("DexOffer::Check(%s) error: wrong countryIso size\n", idTransaction.GetHex().c_str());
            break;
        }
        if (currencyIso.size() != 3) {
            LogPrintf("DexOffer::Check(%s) error:  wrong currencyIso size\n", idTransaction.GetHex().c_str());
            break;
        }
        if (shortInfo.size() > DEX_SHORT_INFO_LENGTH) {
            LogPrintf("DexOffer::Check(%s) error: shortinfo string too long\n", idTransaction.GetHex().c_str());
            break;
        }
        if (details.size() > DEX_DETAILS_LENGTH) {
            LogPrintf("DexOffer::Check(%s) error: details string too long\n", idTransaction.GetHex().c_str());
            break;
        }
        if (type.empty() || (type != OFFER_TYPE_BUY && type != OFFER_TYPE_SELL)) {
            LogPrintf("DexOffer::Check(%s) error: error type\n", idTransaction.GetHex().c_str());
            break;
        }
        if (!defaultCountryIso.isValid(countryIso)) {
            LogPrintf("DexOffer::Check(%s) error: wrong countryIso code\n", idTransaction.GetHex().c_str());
            break;
        }
        if (!defaultCurrencyIso.isValid(currencyIso)) {
            LogPrintf("DexOffer::Check(%s) error: wrong currencyIso code\n", idTransaction.GetHex().c_str());
            break;
        }
        if (!defaultPaymentMethod.isValid(paymentMethod)) {
            LogPrintf("DexOffer::Check(%s) error: wrong payment method\n", idTransaction.GetHex().c_str());
            break;
        }
        if (timeCreate + (timeExpiration * 86400) < (uint64_t)GetTime()) {
            LogPrintf("DexOffer::Check(%s) error: offer expiration time out\n", idTransaction.GetHex().c_str());
            break;
        }
        return true;
    } while (false);
    SetNull();
    return false;
}




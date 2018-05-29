
#include "dexoffer.h"
#include "init.h"
#include "util.h"
#include "utiltime.h"
#include "db/countryiso.h"
#include "db/currencyiso.h"
#include "db/paymentmethodtype.h"
#include "utilstrencodings.h"
#include "parserjsonoffer.h"
#include <univalue.h>
#include <string.h>

namespace dex {

const char * OFFER_TYPE_BUY  =  "buy";
const char * OFFER_TYPE_SELL = "sell";


CurrencyIso  defaultCurrencyIso;
CountryIso   defaultCountryIso;
PaymentMethodType defaultPaymentMethod;


CDexOffer::CDexOffer()
{
    SetNull();
}

CDexOffer::CDexOffer(const CDexOffer &off)
{
    idTransaction    = off.idTransaction;
    hash             = off.hash;
    pubKey           = off.pubKey;
    countryIso       = off.countryIso;
    currencyIso      = off.currencyIso;
    paymentMethod    = off.paymentMethod;
    price            = off.price;
    minAmount        = off.minAmount;
    timeCreate       = off.timeCreate;
    timeExpiration   = off.timeExpiration;
    timeModification = off.timeModification;
    shortInfo        = off.shortInfo;
    details          = off.details;
    editingVersion   = off.editingVersion;
    type             = off.type;
    editsign         = off.editsign;
    myoffer_         = off.myoffer_;
}


CDexOffer::CDexOffer(const dex::OfferInfo &info, dex::TypeOffer offertype)
{
    idTransaction    = info.idTransaction;
    hash             = info.hash;
    pubKey           = info.pubKey;
    countryIso       = info.countryIso;
    currencyIso      = info.currencyIso;
    paymentMethod    = info.paymentMethod;
    price            = info.price;
    minAmount        = info.minAmount;
    timeCreate       = info.timeCreate;
    timeExpiration   = info.timeToExpiration;
    timeModification = info.timeModification;
    shortInfo        = info.shortInfo;
    details          = info.details;
    editingVersion   = info.editingVersion;
    editsign         = info.editsign;
    switch (offertype) {
        case  dex::Buy: type = OFFER_TYPE_BUY;  break;
        case dex::Sell: type = OFFER_TYPE_SELL; break;
    }
    myoffer_ = false;
}

CDexOffer::CDexOffer(const dex::MyOfferInfo &info)
{
    idTransaction    = info.idTransaction;
    hash             = info.hash;
    pubKey           = info.pubKey;
    countryIso       = info.countryIso;
    currencyIso      = info.currencyIso;
    paymentMethod    = info.paymentMethod;
    price            = info.price;
    minAmount        = info.minAmount;
    timeCreate       = info.timeCreate;
    timeExpiration   = info.timeToExpiration;
    timeModification = info.timeModification;
    shortInfo        = info.shortInfo;
    details          = info.details;
    editingVersion   = info.editingVersion;
    editsign         = info.editsign;
    status           = info.status;
    switch (info.type) {
        case  dex::Buy: type = OFFER_TYPE_BUY;  break;
        case dex::Sell: type = OFFER_TYPE_SELL; break;
    }
    myoffer_ = true;
}



void CDexOffer::SetNull()
{
    hash.SetNull();
    idTransaction.SetNull();
    pubKey.clear();
    type.clear();
    countryIso.clear();
    currencyIso.clear();
    paymentMethod = 0;;
    price = 0;
    minAmount = 0;
    timeCreate = 0;
    timeExpiration = 0;
    timeModification = 0;
    shortInfo.clear();
    details.clear();
    editingVersion = 0;
    editsign.clear();
    myoffer_ = false;
}



bool CDexOffer::Create(Type type_, const std::string &pubKey_, const std::string &countryIso_, const std::string &currencyIso_,
           uint8_t paymentMethod_, uint64_t price_, uint64_t minAmount_, time_t timeExpiration_,
           const std::string &shortInfo_, const std::string &details_, const uint32_t &editingVersion_)
{
    uint256 txid;
    return Create(txid, type_, pubKey_, countryIso_, currencyIso_, paymentMethod_, price_, minAmount_, timeExpiration_, shortInfo_, details_, editingVersion_);
}



bool CDexOffer::Create(const uint256 &idTransaction_, Type type_, const std::string &pubKey_, const std::string &countryIso_, const std::string &currencyIso_,
           uint8_t paymentMethod_, uint64_t price_, uint64_t minAmount_, time_t timeExpiration_,
           const std::string &shortInfo_, const std::string &details_, const uint32_t &editingVersion_)
{
    status           = dex::Draft;
    idTransaction    = idTransaction_;
    pubKey           = pubKey_;
    paymentMethod    = paymentMethod_;
    currencyIso      = currencyIso_;
    countryIso       = countryIso_;
    price            = price_;
    minAmount        = minAmount_;
    timeCreate       = GetAdjustedTime();
    timeExpiration   = timeExpiration_;
    timeModification = timeCreate;
    shortInfo        = shortInfo_;
    details          = details_;
    editingVersion   = editingVersion_;
    switch (type_) {
        case  BUY: type = OFFER_TYPE_BUY;  break;
        case SELL: type = OFFER_TYPE_SELL; break;
    }
    if (!Check(false)) {
        SetNull();
        return false;
    }
    hash = MakeHash();
    myoffer_ = false;
    //LogPrint("dex", "Create DexOffer\n%s\n", dump().c_str()); ///< for debug only
    return true;
}


bool CDexOffer::Create(const dex::OfferInfo &info, dex::TypeOffer offertype)
{
    status           = dex::Draft;
    idTransaction    = info.idTransaction;
    pubKey           = info.pubKey;
    paymentMethod    = info.paymentMethod;
    currencyIso      = info.currencyIso;
    countryIso       = info.countryIso;
    price            = info.price;
    minAmount        = info.minAmount;
    timeCreate       = GetAdjustedTime();
    timeExpiration   = info.timeToExpiration;
    timeModification = timeCreate;
    shortInfo        = info.shortInfo;
    details          = info.details;
    editingVersion   = info.editingVersion;
    editsign         = info.editsign;
    switch (offertype) {
        case  dex::Buy: type = OFFER_TYPE_BUY;  break;
        case dex::Sell: type = OFFER_TYPE_SELL; break;
    }
    if (!Check(false)) {
        SetNull();
        return false;
    }
    hash = MakeHash();
    return true;
}

bool CDexOffer::Create(const dex::MyOfferInfo &info)
{
    myoffer_ = true;
    bool ret = Create(info.getOfferInfo(), info.type);
    status = info.status;
    return ret;
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
    info.pubKey           = pubKey;
    info.countryIso       = countryIso;
    info.currencyIso      = currencyIso;
    info.paymentMethod    = paymentMethod;
    info.price            = price;
    info.minAmount        = minAmount;
    info.timeCreate       = timeCreate;
    info.timeToExpiration = timeExpiration;
    info.timeModification = timeModification;
    info.shortInfo        = shortInfo;
    info.details          = details;
    info.editingVersion   = editingVersion;
    info.editsign         = editsign;
    return info;
}

CDexOffer::operator dex::MyOfferInfo() const
{
    dex::MyOfferInfo info;
    info.idTransaction    = idTransaction;
    info.hash             = hash;
    info.pubKey           = pubKey;
    info.countryIso       = countryIso;
    info.currencyIso      = currencyIso;
    info.paymentMethod    = paymentMethod;
    info.price            = price;
    info.minAmount        = minAmount;
    info.timeCreate       = timeCreate;
    info.timeToExpiration = timeExpiration;
    info.timeModification = timeModification;
    info.shortInfo        = shortInfo;
    info.details          = details;
    info.editingVersion   = editingVersion;
    info.type             = getTypeOffer();
    info.editingVersion   = editingVersion;
    info.editsign         = editsign;
    info.status           = (dex::StatusOffer)status;
    return info;
}


CDexOffer& CDexOffer::operator=(const CDexOffer& off)
{
    idTransaction    = off.idTransaction;
    hash             = off.hash;
    pubKey           = off.pubKey;
    countryIso       = off.countryIso;
    currencyIso      = off.currencyIso;
    paymentMethod    = off.paymentMethod;
    price            = off.price;
    minAmount        = off.minAmount;
    timeCreate       = off.timeCreate;
    timeExpiration   = off.timeExpiration;
    timeModification = off.timeModification;
    shortInfo        = off.shortInfo;
    details          = off.details;
    editingVersion   = off.editingVersion;
    type             = off.type;
    myoffer_         = off.myoffer_;
    status           = off.status;
    editsign         = off.editsign;
    return *this;
}

std::string CDexOffer::getType() const
{
    return type;
}

dex::TypeOffer CDexOffer::getTypeOffer() const
{
    if (type == OFFER_TYPE_BUY)  {
        return dex::Buy;
    } else {
        return dex::Sell;
    }
}


bool CDexOffer::isBuy() const
{
    return type == OFFER_TYPE_BUY;
}


bool CDexOffer::isSell() const
{
    return type == OFFER_TYPE_SELL;
}

bool CDexOffer::isMyOffer() const
{
    return myoffer_;
}



std::string CDexOffer::dump() const
{
    return strprintf("CDexOffer::dump()\n"
        "\ttype\t\t%s\n"
        "\tidTransaction\t%s\n"
        "\thash\t\t%s\n"
        "\tpubKey\t\t%s\n"
        "\tcountryIso\t%s\n"
        "\tcurrencyIso\t%s\n"
        "\tpaymentMethod\t%d\n"
        "\tprice\t\t%lld\n"
        "\tminAmount\t%lld\n"
        "\ttimeCreate\t%lld\n"
        "\ttimeExpiration\t%lld\n"
        "\ttimeModification\t%lld\n"
        "\tshortInfo\t%s\n"
        "\tdetails\t\t%s\n"
        "\teditingVersion\t\t%d\n"
        "\teditsign\t\t%s\n"
        "\tstatus\t\t%d\n",
        type.c_str(), idTransaction.GetHex().c_str(), hash.GetHex().c_str(), pubKey.c_str(),
        countryIso.c_str(), currencyIso.c_str(), paymentMethod, price, minAmount, timeCreate,
                     timeExpiration, timeModification, shortInfo.c_str(), details.c_str(), editingVersion, editsign.c_str(), status);
}

CPubKey CDexOffer::getPubKeyObject() const
{
    std::vector<unsigned char> data(ParseHex(pubKey));
    CPubKey key(data.begin(), data.end());
    return key;
}

bool CDexOffer::Check(bool fullcheck)
{
    int fine = 0;
    return Check(fullcheck, fine);
}

bool CDexOffer::Check(bool fullcheck, int &fine)
{
    fine = 20;
    do {
        if (fullcheck && idTransaction.IsNull()) {
            LogPrint("dex", "DexOffer::Check error: idTransaction is empty\n");
            break;
        }
        if (fullcheck && hash.IsNull()) {
            LogPrint("dex", "DexOffer::Check(transactionId %s) error: hash is empty\n", idTransaction.GetHex().c_str());
            break;
        }
        if (fullcheck && !getPubKeyObject().IsFullyValid()) {
            LogPrint("dex", "DexOffer::Check(%s) error: pubKey is invalid\n", hash.GetHex().c_str());
        }
        if (fullcheck && hash != MakeHash()) {
            LogPrint("dex", "DexOffer::Check(%s) error: hash not equal\n", hash.GetHex().c_str());
            break;
        }
        if (countryIso.size() != 2) {
            LogPrint("dex", "DexOffer::Check(%s) error: wrong countryIso size\n", hash.GetHex().c_str());
            break;
        }
        if (currencyIso.size() != 3) {
            LogPrint("dex", "DexOffer::Check(%s) error:  wrong currencyIso size\n", hash.GetHex().c_str());
            break;
        }
        if (numberSings(shortInfo) > DEX_SHORT_INFO_LENGTH) {
            LogPrint("dex", "DexOffer::Check(%s) error: shortinfo string too long\n", hash.GetHex().c_str());
            break;
        }
        if (numberSings(details) > DEX_DETAILS_LENGTH) {
            LogPrint("dex", "DexOffer::Check(%s) error: details string too long\n", hash.GetHex().c_str());
            break;
        }
        if (type.empty() || (type != OFFER_TYPE_BUY && type != OFFER_TYPE_SELL)) {
            LogPrint("dex", "DexOffer::Check(%s) error: error type\n", idTransaction.GetHex().c_str());
            break;
        }
        if (!defaultCountryIso.isValid(countryIso)) {
            LogPrint("dex", "DexOffer::Check(%s) error: wrong countryIso code\n", hash.GetHex().c_str());
            break;
        }
        if (!defaultCurrencyIso.isValid(currencyIso)) {
            LogPrint("dex", "DexOffer::Check(%s) error: wrong currencyIso code\n", hash.GetHex().c_str());
            break;
        }
        if (!defaultPaymentMethod.isValid(paymentMethod)) {
            LogPrint("dex", "DexOffer::Check(%s) error: wrong payment method\n", hash.GetHex().c_str());
            break;
        }
        if ((timeExpiration) < static_cast<uint64_t>(GetTime())) {
            LogPrint("dex", "DexOffer::Check(%s) error: offer expiration time out\n", hash.GetHex().c_str());
            break;
        }
        if (editingVersion == 0 && timeCreate != timeModification) {
            fine = 5;
            LogPrint("dex", "DexOffer::Check(%s) error: wrong modification time\n", hash.GetHex().c_str());
            break;
        }
        fine = 0;
        return true;
    } while (false);
    SetNull();
    return false;
}

bool CDexOffer::CheckModTimeOnEdit(){
    if (timeModification > GetAdjustedTime() + 3600 || timeModification < GetAdjustedTime() - 3600) {
        LogPrint("dex", "DexOffer::CheckModTimeOnEdit(%s) error: wrong modification time\n", hash.GetHex().c_str());
        return false;
    }   
    return true;
}

UniValue CDexOffer::getUniValue()
{
    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("type", type));
    result.push_back(Pair("idTransaction", idTransaction.GetHex()));
    result.push_back(Pair("hash", hash.GetHex()));
    result.push_back(Pair("pubKey", pubKey));
    result.push_back(Pair("countryIso", countryIso));
    result.push_back(Pair("currencyIso", currencyIso));
    result.push_back(Pair("paymentMethod", paymentMethod));
    result.push_back(Pair("price", price));
    result.push_back(Pair("minAmount", minAmount));
    result.push_back(Pair("timeCreate", timeCreate));
    result.push_back(Pair("timeExpiration", timeExpiration));
    result.push_back(Pair("timeModification", timeModification));
    result.push_back(Pair("shortInfo", shortInfo));
    result.push_back(Pair("details", details));
    result.push_back(Pair("editingVersion", static_cast<uint64_t>(editingVersion)));
    result.push_back(Pair("editSign", editsign));
    return result;
}


uint256 CDexOffer::MakeEditHash()
{
    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    ss << hash;
    ss << idTransaction;
    ss << price;
    ss << shortInfo;
    ss << details;
    ss << editingVersion;
    ss << timeModification;
    return ss.GetHash();
}


bool CDexOffer::CheckEditSign()
{
    do {
        if (IsNull()) {
            LogPrint("dex", "DexOffer::CheckEditSign() error: offer is Null\n" );
            break;
        }

        if (editsign.empty()) {
            LogPrint("dex", "DexOffer::CheckEditSign(%s) error: edit sign is empty\n", hash.GetHex().c_str());
            break;
        }
        std::vector<unsigned char> vchSign = ParseHex(editsign);

        CPubKey pkey = getPubKeyObject();
        if (!pkey.IsFullyValid()) {
            LogPrint("dex", "DexOffer::CheckEditSign(%s) error: invalid pub key\n", hash.GetHex().c_str());
            break;
        }

        uint256 edithash = MakeEditHash();

        if (!pkey.Verify(edithash, vchSign)) {
            LogPrint("dex", "DexOffer::CheckEditSign(%s) error: invalid offer editsign\n", hash.GetHex().c_str());
            break;
        }
        return true;
    } while (false);
    return false;
}

}

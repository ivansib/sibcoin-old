#ifndef DEXDTO_H
#define DEXDTO_H

#include "uint256.h"
#include <algorithm>

namespace dex {

const int numberOfDecimalsForPrice = 8;
static const int DEX_SHORT_INFO_LENGTH = 140;
static const int DEX_DETAILS_LENGTH = 768;

struct CountryInfo {
    std::string iso;
    std::string name;
    bool enabled;
};

struct CurrencyInfo {
    std::string iso;
    std::string name;
    std::string symbol;
    bool enabled;
};

struct PaymentMethodInfo {
    unsigned char type;
    std::string name;
    std::string description;
};

struct OfferInfo {
    uint256 idTransaction;
    uint256 hash;
    std::string pubKey;
    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;
    uint64_t price;
    uint64_t minAmount;
    uint64_t timeCreate;
    uint64_t timeToExpiration;
    std::string shortInfo;
    std::string details;
    int editingVersion;
    std::string editsign;
};

enum TypeOffer {
    Buy,
    Sell
};

enum StatusOffer {
    Indefined,
    Active,
    Draft,
    Expired,
    Cancelled,
    Suspended,
    Unconfirmed
};

class CStatusOffer {
public:
    std::vector<std::string> statuses_;
    StatusOffer status;

public:
    CStatusOffer() : statuses_({"Indefined","Active","Draft","Expired","Cancelled","Suspended","Unconfirmed"}) { 
      status = Indefined;
    };

    StatusOffer get() const { return status; };
    std::string str() const { return status2str(status); };
    operator StatusOffer() const { return status; };
    operator std::string() const { return status2str(status); };

    void set(StatusOffer status) { this->status = status; };
    void set(const std::string &strstatus) { status = str2status(strstatus); };
    CStatusOffer &operator =(const StatusOffer &status) { set(status); return *this; };
    CStatusOffer &operator =(const std::string &str) { set(str); return *this; };

    std::string status2str(const StatusOffer status)  const { return statuses_[status]; };
    StatusOffer str2status(const std::string &strstatus) const {
        for (size_t i = 0; i < statuses_.size(); i++) {
            if (std::equal(strstatus.begin(), strstatus.end(), statuses_[i].begin(), isequal) == 0) {
                return static_cast<StatusOffer>(i);
            }
        }
        return Indefined;
    };

    static bool isequal(int c1, int c2)
    {
        return std::toupper(c1) == std::toupper(c2);
    };


};

enum TypeTable {
    Countries,
    Currencies,
    PaymentMethods,
    OffersSell,
    OffersBuy,
    MyOffers,
    FiltersList
};

enum TypeTableOperation {
    Read,
    Add,
    Edit,
    Delete
};

enum StatusTableOperation {
    Ok,
    Error
};

struct MyOfferInfo : OfferInfo {
    TypeOffer type;
    StatusOffer status;

    OfferInfo getOfferInfo() const {
        OfferInfo info;

        info.idTransaction = idTransaction;
        info.hash = hash;
        info.pubKey = pubKey;
        info.countryIso = countryIso;
        info.currencyIso = currencyIso;
        info.paymentMethod = paymentMethod;
        info.price = price;
        info.minAmount = minAmount;
        info.timeCreate = timeCreate;
        info.timeToExpiration = timeToExpiration;
        info.shortInfo = shortInfo;
        info.details = details;
        info.editingVersion = editingVersion;
        info.editsign = editsign;

        return info;
    }

    void setOfferInfo(const OfferInfo &info) {
        idTransaction = info.idTransaction;
        hash = info.hash;
        pubKey = info.pubKey;
        countryIso = info.countryIso;
        currencyIso = info.currencyIso;
        paymentMethod = info.paymentMethod;
        price = info.price;
        minAmount = info.minAmount;
        timeCreate = info.timeCreate;
        timeToExpiration = info.timeToExpiration;
        shortInfo = info.shortInfo;
        details = info.details;
        editingVersion = info.editingVersion;
        editsign = info.editsign;
    }
};

}

#endif

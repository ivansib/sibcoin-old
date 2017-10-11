#ifndef DEXDTO_H
#define DEXDTO_H

#include "uint256.h"

namespace dex {

struct CountryInfo {
    std::string name;
    bool enabled;
};

struct CurrencyInfo {
    std::string name;
    std::string symbol;
    bool enabled;
};

struct PaymentMethodInfo {
    std::string name;
    std::string description;
};

struct OfferInfo {
    uint256 idTransaction;
    uint256 hash;
    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;
    uint64_t price;
    uint64_t minAmount;
    uint64_t timeCreate;
    uint64_t timeExpiration;
    std::string shortInfo;
    std::string details;
};

}

#endif

#include <univalue.h>
#include "parserjsonoffer.h"
#include "db/countryiso.h"
#include "db/currencyiso.h"

MyOfferInfo fromJsonForAdd(const std::string &json, std::string &error)
{
    MyOfferInfo offer;

    UniValue uv(UniValue::VOBJ);
    uv.read(json);

    std::string strType = uv["type"].get_str();

    if (strType != "buy" && strType != "sell") {
        error = "offer type is empty";
        return MyOfferInfo();
    }

    offer.type = Buy;
    if (strType == "sell") {
        offer.type = Sell;
    }

    offer.countryIso = uv["countryIso"].get_str();
    dex::CountryIso countryiso;
    if (!countryiso.isValid(offer.countryIso)) {
        error = "invalid country code";
        return MyOfferInfo();
    }

    offer.currencyIso = uv["currencyIso"].get_str();
    dex::CurrencyIso currencyiso;
    if (!currencyiso.isValid(offer.currencyIso)) {
        error = "invalid currency code";
        return MyOfferInfo();
    }

    offer.paymentMethod = uv["paymentMethod"].get_int();
    if (offer.paymentMethod != 1 && offer.paymentMethod != 128) {
        error = "invalid paymentMethod";
        return MyOfferInfo();
    }

    offer.price = uv["price"].get_int();
    offer.minAmount = uv["minAmount"].get_int();

    offer.timeToExpiration = uv["timeToExpiration"].get_int();
    if (offer.timeToExpiration != 10 && offer.timeToExpiration != 20 && offer.timeToExpiration != 30) {
        error = "invalid timeExpiration";
        return MyOfferInfo();
    }

    offer.shortInfo = uv["shortInfo"].get_str();
    offer.details = uv["details"].get_str();

    return offer;
}

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "parserjsonoffer.h"
#include "db/countryiso.h"
#include "db/currencyiso.h"

MyOfferInfo fromJsonForAdd(const std::string &json, std::string &error)
{
    std::stringstream ss;
    ss << json;
    namespace pt = boost::property_tree;
    pt::ptree root;

    MyOfferInfo offer;

    try {
        pt::read_json(ss, root);
    } catch(boost::property_tree::json_parser::json_parser_error &je) {
        error = "Error parsing: " + je.filename() + " on line: " + std::to_string(je.line()) + "\n";
        error += je.message() + "\n";
        return MyOfferInfo();
    }

    std::string strType = root.get<std::string>("type");

    if (strType != "buy" && strType != "sell") {
        error = "offer type is empty";
        return MyOfferInfo();
    }

    offer.type = Buy;
    if (strType == "sell") {
        offer.type = Sell;
    }

    offer.countryIso = root.get<std::string>("countryIso");
    dex::CountryIso countryiso;
    if (!countryiso.isValid(offer.countryIso)) {
        error = "invalid country code";
        return MyOfferInfo();
    }

    offer.currencyIso = root.get<std::string>("currencyIso");
    dex::CurrencyIso currencyiso;
    if (!currencyiso.isValid(offer.currencyIso)) {
        error = "invalid currency code";
        return MyOfferInfo();
    }

    offer.paymentMethod = root.get<uint8_t>("paymentMethod", 0);
    if (offer.paymentMethod != 1 && offer.paymentMethod != 128) {
        error = "invalid paymentMethod";
        return MyOfferInfo();
    }

    offer.price = root.get<int>("price", 0);
    offer.minAmount = root.get<int>("minAmount", 0);

    offer.timeToExpiration = root.get<int>("timeToExpiration", 0);
    if (offer.timeToExpiration != 10 && offer.timeToExpiration != 20 && offer.timeToExpiration != 30) {
        error = "invalid timeExpiration";
        return MyOfferInfo();
    }

    offer.shortInfo = root.get<std::string>("shortInfo");
    offer.details = root.get<std::string>("details");

    return offer;
}

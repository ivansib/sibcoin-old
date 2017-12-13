
#include "rpcserver.h"

#include "clientversion.h"
#include "main.h"
#include "net.h"
#include "netbase.h"
#include "protocol.h"
#include "sync.h"
#include "timedata.h"
#include "util.h"
#include "utilstrencodings.h"
#include "version.h"
#include <univalue.h>
#include "streams.h"
#include <algorithm>
#include "dexoffer.h"
#include "random.h"
#include "dex/dexdb.h"
#include "dex.h"
#include "core_io.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "dextransaction.h"
#include "db/countryiso.h"
#include "db/currencyiso.h"
#include "db/defaultdatafordb.h"

using namespace std;

/*

UniValue testdexoffer(const UniValue& params, bool fHelp)
{
    if (!fTxIndex) {
        throw runtime_error(
            "To use this feture please enable -txindex and make -reindex.\n"
        );
    }

    if (fHelp || params.size() > 1 || (params[0].get_str() != "buy" && params[0].get_str() != "sell"))
        throw runtime_error(
            "testdexoffer buy|sell \n"
            "Create TEST dex offer and broadcast it.\n"
        );

    CDex dex;
    uint256 tr_id = GetRandHash();
    CDexOffer::Type type;
    if (params[0].get_str() == "buy") type = CDexOffer::BUY;
    if (params[0].get_str() == "sell") type = CDexOffer::SELL;

    if (!dex.CreateOffer(type, tr_id, "RU", "RUB", 1, 10000, 20000 , 10, "test shortinfo", "test details"))
        throw runtime_error("dexoffer not create\n someting wrong\n");

    LOCK2(cs_main, cs_vNodes);
    BOOST_FOREACH(CNode* pNode, vNodes) {
        pNode->PushMessage(NetMsgType::DEXOFFBCST, dex.offer);
    }

    return NullUniValue;
}



UniValue payoffertx(const UniValue& params, bool fHelp)
{
    if (!fTxIndex) {
        throw runtime_error(
            "To use this feture please enable -txindex and make -reindex.\n"
        );
    }

    if (fHelp)
        throw runtime_error(
            "payoffertx  \n"
            "WARNING!!! Create TEST payoffer transaction \n"
        );

    std::string error;

    CDex offer;
    offer.CreateOffer(CDexOffer::SELL, "RU", "RUB", 1, 100, 1000, 100, "test dex transaction", "dex offer for test transaction");


    uint256 tx;
    if (!offer.PayForOffer(tx, error))
            throw runtime_error(error.c_str());

    offer.CheckOfferTx(error);
    std::string result = strprintf("offer:\n%s\ntx %s\nerror %s", offer.offer.dump().c_str(), tx.GetHex().c_str(), error.c_str());

    throw runtime_error(result.c_str());

    return NullUniValue;
}
*/


UniValue dexoffers(const UniValue& params, bool fHelp)
{
    if (!fTxIndex) {
        throw runtime_error(
            "To use this feture please enable -txindex and make -reindex.\n"
        );
    }

    if (fHelp || params.size() < 1 || params.size() > 4)
        throw runtime_error(
            "dexoffers [buy|sell|all] [country] [currency] [payment_method]\n"
            "Get DEX offers list.\n"

            "\nArguments:\n"
            "NOTE: Any of the parameters may be skipped.You must specify at least one parameter.\n"
            "\tcountry         (string, optional) two-letter country code (ISO 3166-1 alpha-2 code).\n"
            "\tcurrency        (string, optional) three-letter currency code (ISO 4217).\n"
            "\tpayment_method  (string, optional) payment method name.\n"

            "\nResult:\n"

            "\nExamples:\n"
            + HelpExampleCli("dexoffers", "all USD")
            + HelpExampleCli("dexoffers", "RU RUB cash")
            + HelpExampleCli("dexoffers", "all USD online")
        );

    UniValue result(UniValue::VARR);

    std::string typefilter, countryfilter, currencyfilter;
    std::string methodfilter = "*";
    unsigned char methodfiltertype = 0;
    std::list<std::string> words {"buy", "sell", "all"};
    dex::CountryIso  countryiso;
    dex::CurrencyIso currencyiso;
    dex::DexDB db(strDexDbFile);

    for (size_t i = 0; i < params.size(); i++) {
        if (i == 0 && typefilter.empty()) {
            if (std::find(words.begin(), words.end(), params[0].get_str()) != words.end()) {
                typefilter = params[0].get_str();
                continue;
            } else {
                typefilter = "all";
            }
        }
        if (i < 2 && countryfilter.empty()) {
            if (countryiso.isValid(params[i].get_str())) {
                countryfilter = params[i].get_str();
                continue;
            } else {
                countryfilter = "*";
            }
        }
        if (i < 3 && currencyfilter.empty()) {
            if (currencyiso.isValid(params[i].get_str())) {
                currencyfilter = params[i].get_str();
                continue;
            } else {
                currencyfilter = "*";
            }
        }
        {
            methodfilter.clear();
            std::string methodname = boost::algorithm::to_lower_copy(params[i].get_str());
            std::list<dex::PaymentMethodInfo> pms = db.getPaymentMethodsInfo();
            for (auto j : pms) {
                std::string name = boost::algorithm::to_lower_copy(j.name);
                if (name == methodname) {
                    methodfilter = j.name;
                    methodfiltertype = j.type;
                }
            }

            if (methodfilter.empty()) {
                throw runtime_error("\nwrong parameter: " + params[i].get_str() + "\n");
            }
        }
    }

    if (currencyfilter.empty()) currencyfilter = "*";
    if (countryfilter.empty()) countryfilter = "*";

    if (countryfilter.empty() || currencyfilter.empty() || typefilter.empty() || methodfilter.empty()) {
        throw runtime_error("\nwrong parameters\n");
    }

    // check country and currency in DB
    if (countryfilter != "*") {
        dex::CountryInfo  countryinfo = db.getCountryInfo(countryfilter);
        if (!countryinfo.enabled) {
            throw runtime_error("\nERROR: this country is disabled in DB\n");
        }
    }
    if (currencyfilter != "*") {
        dex::CurrencyInfo  currencyinfo = db.getCurrencyInfo(currencyfilter);
        if (!currencyinfo.enabled) {
            throw runtime_error("\nERROR: this currency is disabled in DB\n");
        }
    }



    if (typefilter == "buy" || typefilter == "all") {
        std::list<dex::OfferInfo> offers = db.getOffersBuy();
        for (auto i : offers) {
          if (countryfilter != "*"  && countryfilter != i.countryIso) continue;
          if (currencyfilter != "*" && currencyfilter != i.currencyIso) continue;
          if (methodfilter != "*"   && methodfiltertype != i.paymentMethod) continue;
          CDexOffer o(i, dex::Buy);
          result.push_back(o.getUniValue());
        }
    }

    if (typefilter == "sell" || typefilter == "all") {
        std::list<dex::OfferInfo> offers = db.getOffersSell();
        for (auto i : offers) {
          if (countryfilter != "*"  && countryfilter != i.countryIso) continue;
          if (currencyfilter != "*" && currencyfilter != i.currencyIso) continue;
          if (methodfilter != "*"   && methodfiltertype != i.paymentMethod) continue;
          CDexOffer o(i, dex::Sell);
          result.push_back(o.getUniValue());
        }
    }

    return result;
}




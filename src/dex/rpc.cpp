
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

#include "dexoffer.h"
#include "random.h"
#include "dex/dexdb.h"
#include "dex.h"
#include "core_io.h"
#include <boost/foreach.hpp>


#include "dextransaction.h"

using namespace std;


UniValue dexoffer(const UniValue& params, bool fHelp)
{

    if (fHelp || params.size() > 1 || (params[0].get_str() != "buy" && params[0].get_str() != "sell"))
        throw runtime_error(
            "dexoffer buy|sell \n"
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



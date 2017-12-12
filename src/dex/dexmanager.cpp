
#include "dexmanager.h"

#include "init.h"
#include "util.h"
#include "utilstrencodings.h"

#include "dex/dexdb.h"
#include "txmempool.h"
#include "base58.h"




#define PAYOFFER_RETURN_FEE     10000
#define PAYOFFER_TX_FEE         50000000
#define PAYOFFER_MIN_TX_HEIGHT  6


CDexManager dexman;


CDexManager::CDexManager()
{
}



void CDexManager::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (strCommand == NetMsgType::DEXOFFBCST) {
        CDexOffer offer;
        vRecv >> offer;
        if (offer.Check(true)) {
            CDex dex(offer);
            std::string error;
            if (dex.CheckOfferTx(error)) {
                dex::DexDB db(strDexDbFile);
                bool bFound = false;
                if (offer.isBuy())  {
                    if (db.isExistOfferBuy(offer.idTransaction)) {
                      bFound = true;
                    } else {
                        db.addOfferBuy(offer);
                    }
                }

                if (offer.isSell())  {
                    if (db.isExistOfferSell(offer.idTransaction)) {
                      bFound = true;
                    } else {
                        db.addOfferSell(offer);
                    }
                }

                if (!bFound) { // need to save and relay
                    LOCK2(cs_main, cs_vNodes);
                    BOOST_FOREACH(CNode* pNode, vNodes) {
                        pNode->PushMessage(NetMsgType::DEXOFFBCST, offer);
                    }
                }
                LogPrintf("DEXOFFBCST --\n%s\nfound %d\n", offer.dump().c_str(), bFound);
            } else {
                LogPrintf("DEXOFFBCST --check offer tx fail(%s)\n", offer.idTransaction.GetHex().c_str());
            }
        } else {
            LogPrintf("DEXOFFBCST -- offer check fail\n");
        }
    }
}

void CDexManager::sendOffer(const CDexOffer &offer)
{
    LOCK2(cs_main, cs_vNodes);

    for (CNode *pNode : vNodes) {
        pNode->PushMessage(NetMsgType::DEXOFFBCST, offer);
    }
}




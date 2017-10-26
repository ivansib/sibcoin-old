
#include "dexmanager.h"

#include "init.h"
#include "util.h"

#include "dexoffer.h"
#include "dex/dexdb.h"


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
            dex::DexDB db(strDexDbFile);
            bool bFound = false;
            if (offer.isBuy())  {
                if (db.isExistOfferBuy(offer.idTransaction.GetHex())) {
                  bFound = true;
                } else {
                    db.addOfferBuy(offer);
                }
            }

            if (offer.isSell())  {
                if (db.isExistOfferSell(offer.idTransaction.GetHex())) {
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
            LogPrint("dex", "DEXOFFBCST --\n%s\nfound %d\n", offer.dump().c_str(), bFound);
        }
    }
}



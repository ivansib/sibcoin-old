
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

        //pfrom->setAskFor.erase(mnb.GetHash());

        dex::DexDB db(strDexDbFile);
        if (offer.isBuy())  db.addOfferBuy(offer);
        if (offer.isSell()) db.addOfferSell(offer);

        LogPrint("dex", "DEXOFFBCST --\n%s\n", offer.dump().c_str());

/*
        int nDos = 0;

        if (CheckMnbAndUpdateMasternodeList(pfrom, mnb, nDos)) {
            // use announced Masternode as a peer
            addrman.Add(CAddress(mnb.addr), pfrom->addr, 2*60*60);
        } else if(nDos > 0) {
            Misbehaving(pfrom->GetId(), nDos);
        }

        if(fMasternodesAdded) {
            NotifyMasternodeUpdates();
        }
*/

    }
}



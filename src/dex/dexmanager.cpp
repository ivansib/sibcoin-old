
#include "dexmanager.h"

#include "init.h"
#include "util.h"



CDexManager dexman;




CDexManager::CDexManager()
{
}



void CDexManager::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (strCommand == NetMsgType::DEXOFFBCST) {

        CDexBroadcast dexbc;
        vRecv >> dexbc;

        //pfrom->setAskFor.erase(mnb.GetHash());

        LogPrint("dex", "DEXOFFBCST -- \n");

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



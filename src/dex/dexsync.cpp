#include "dexsync.h"
#include "dexmanager.h"
#include "masternode-sync.h"

CDexSync dexsync;

CDexSync::CDexSync()
{
    db = nullptr;
    isSync = false;
}

CDexSync::~CDexSync()
{
    if (db != nullptr) {
        db->freeInstance();
    }
}

void CDexSync::ProcessMessage(CNode *pfrom, std::string &strCommand, CDataStream &vRecv)
{
    initDB();

    if (strCommand == NetMsgType::DEXSYNCGETALLHASH) {
        sendHashOffers(pfrom);
    } else if (strCommand == NetMsgType::DEXSYNCALLHASH) {
        getHashsAndSendRequestForGetOffers(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCGETOFFER) {
        sendOffer(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCOFFER) {
        getOfferAndSaveInDb(vRecv);
    }
}

void CDexSync::startSyncDex()
{
    LogPrintf("ThreadDexManager -- start synchronization offers\n");
    std::vector<CNode*> vNodesCopy = CopyNodeVector();

    for (auto node : vNodesCopy) {
        if(node->fMasternode || (fMasterNode && node->fInbound)) {
            continue;
        }

        node->PushMessage(NetMsgType::DEXSYNCGETALLHASH);
    }

    isSync = true;
}

bool CDexSync::isSynced()
{
    return isSync;
}

void CDexSync::initDB()
{
    if (db == nullptr) {
        db = DexDB::instance();
    }
}

void CDexSync::sendHashOffers(CNode *pfrom) const
{
    LogPrintf("DEXSYNCGETALLHASH -- receive request on send list pairs hashe and version from %s\n", pfrom->addr.ToString());
    auto hvs = dexman.availableOfferHashAndVersion();

    if (hvs.size() > 0) {
        LogPrintf("DEXSYNCGETALLHASH -- send list pairs hashe and version\n");
        pfrom->PushMessage(NetMsgType::DEXSYNCALLHASH, hvs);
    }
}

void CDexSync::getHashsAndSendRequestForGetOffers(CNode *pfrom, CDataStream &vRecv) const
{
    LogPrintf("DEXSYNCALLHASH -- get list hashes from %s\n", pfrom->addr.ToString());

    std::list<std::pair<uint256, int>>  nodeHvs;
    vRecv >> nodeHvs;
    auto hvs = dexman.availableOfferHashAndVersion();

    for (auto h : nodeHvs) {
        auto found = std::find_if(hvs.begin(), hvs.end(), [h](std::pair<uint256, int> item){ return item.first == h.first; });

        auto isSend = false;

        if (found != hvs.end()) {
            if (h.second > found->second) {
                isSend = true;
            }
        } else {
            isSend = true;
        }

        if (isSend) {
            LogPrintf("DEXSYNCALLHASH -- send a request for get offer info with hash = %s\n", h.first.GetHex().c_str());
            pfrom->PushMessage(NetMsgType::DEXSYNCGETOFFER, h);
        }
    }
}

void CDexSync::sendOffer(CNode *pfrom, CDataStream &vRecv) const
{
    LogPrintf("DEXSYNCGETOFFER -- receive request on send offer from %s\n", pfrom->addr.ToString());

    uint256 hash;
    vRecv >> hash;

    auto offer = dexman.getOfferInfo(hash);

    if (offer.Check(true)) {
        LogPrintf("DEXSYNCGETOFFER -- send offer info with hash = %s\n", hash.GetHex().c_str());
        pfrom->PushMessage(NetMsgType::DEXSYNCOFFER, offer);
    }
}

void CDexSync::getOfferAndSaveInDb(CDataStream &vRecv)
{
    CDexOffer offer;
    vRecv >> offer;

    LogPrintf("DEXSYNCOFFER -- get offer info with hash = %s\n", offer.hash.GetHex().c_str());

    if (offer.Check(true)) {
        CDex dex(offer);
        std::string error;
        if (dex.CheckOfferTx(error)) {
            if (offer.isBuy())  {
                if (db->isExistOfferBuy(offer.idTransaction)) {
                    OfferInfo existOffer = db->getOfferBuy(offer.idTransaction);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferBuy(offer);
                    }
                } else {
                    db->addOfferBuy(offer);
                }
            } else if (offer.isSell())  {
                if (db->isExistOfferSell(offer.idTransaction)) {
                    OfferInfo existOffer = db->getOfferSell(offer.idTransaction);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferSell(offer);
                    }
                } else {
                    db->addOfferSell(offer);
                }
            }
        } else {
            if (dexman.getUncOffers()->isExistOffer(offer.hash)) {
                OfferInfo existOffer = dexman.getUncOffers()->getOffer(offer.hash);
                if (offer.editingVersion > existOffer.editingVersion) {
                    dexman.getUncOffers()->deleteOffer(offer.hash);
                    dexman.getUncOffers()->setOffer(offer);
                }
            } else {
                dexman.getUncOffers()->setOffer(offer);
            }
        }
    }
}

void DexConnectSignals()
{
    masternodeSync.syncFinished.connect(boost::bind(&CDexSync::startSyncDex, &dexsync));
}

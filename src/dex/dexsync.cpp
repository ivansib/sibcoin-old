#include <set>
#include "dexsync.h"
#include "dexmanager.h"
#include "masternode-sync.h"
#include "masternodeman.h"
#include "init.h"
#include "ui_interface.h"
#include "pubkey.h"
#include "wallet.h"

CDexSync dexsync;

const int MIN_NUMBER_DEX_NODE = 4;
const int MIN_NUMBER_DEX_NODE_TESTNET = 2;

CDexSync::CDexSync()
{
    status = NoStarted;
    db = nullptr;
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
    if (status == NoStarted) {
        uiInterface.NotifyAdditionalDataSyncProgressChanged(0);
    }

    if (!canStart() || status != NoStarted) {
        return;
    }

    status = Started;

    LogPrint(NULL, "startSyncDex -- start synchronization offers\n");
    maxOffersNeedDownload = 0;
    auto vNodesCopy = CopyNodeVector();

    for (auto node : vNodesCopy) {
        if (node->nVersion < MIN_DEX_VERSION) {
            continue;
        }

        if (!mnodeman.isExist(node)) {
            continue;
        }

        if(node->fMasternode || (fMasterNode && node->fInbound)) {
            continue;
        }

        node->PushMessage(NetMsgType::DEXSYNCGETALLHASH);
    }

    status = Initial;

    ReleaseNodeVector(vNodesCopy);

    Timer timer(30000, FinishSyncDex);
}

void CDexSync::finishSyncDex()
{
    status = Finished;
    uiInterface.NotifyAdditionalDataSyncProgressChanged(1);
}

bool CDexSync::isSynced() const
{
    return status == Finished;
}

std::string CDexSync::getSyncStatus() const
{
    std::string str;
    switch (status) {
    case NoStarted:
    case Started:
    case Initial:
        str = _("Synchronization offers pending...");
        break;
    case Sync:
        str = _("Synchronization offers...");
        break;
    case Finished:
        str = _("Synchronization offers finished");
        break;
    default:
        break;
    }

    return str;
}

CDexSync::Status CDexSync::statusSync()
{
    return status;
}

int CDexSync::minNumDexNode() const
{
    int minNumDexNode = MIN_NUMBER_DEX_NODE;
    if (Params().NetworkIDString() == CBaseChainParams::TESTNET) {
        minNumDexNode = MIN_NUMBER_DEX_NODE_TESTNET;
    }

    return minNumDexNode;
}

void CDexSync::initDB()
{
    if (db == nullptr) {
        db = DexDB::instance();
    }
}

void CDexSync::sendHashOffers(CNode *pfrom) const
{
    LogPrint("dex", "DEXSYNCGETALLHASH -- receive request on send list pairs hashe and version from %s\n", pfrom->addr.ToString());
    auto hvs = dexman.availableOfferHashAndVersion();

    if (hvs.size() > 0) {
        LogPrint("dex", "DEXSYNCGETALLHASH -- send list pairs hashe and version\n");
        pfrom->PushMessage(NetMsgType::DEXSYNCALLHASH, hvs);
    }
}

void CDexSync::getHashsAndSendRequestForGetOffers(CNode *pfrom, CDataStream &vRecv)
{
    LogPrint("dex", "DEXSYNCALLHASH -- get list hashes from %s\n", pfrom->addr.ToString());

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
            insertItemFromOffersNeedDownload(h.first);
            LogPrint("dex", "DEXSYNCALLHASH -- send a request for get offer info with hash = %s\n", h.first.GetHex().c_str());
            pfrom->PushMessage(NetMsgType::DEXSYNCGETOFFER, h);
        }
    }
}

void CDexSync::sendOffer(CNode *pfrom, CDataStream &vRecv) const
{
    LogPrint("dex", "DEXSYNCGETOFFER -- receive request on send offer from %s\n", pfrom->addr.ToString());

    uint256 hash;
    vRecv >> hash;

    auto offer = dexman.getOfferInfo(hash);

    if (offer.Check(true)) {
        LogPrint("dex", "DEXSYNCGETOFFER -- send offer info with hash = %s\n", hash.GetHex().c_str());
        pfrom->PushMessage(NetMsgType::DEXSYNCOFFER, offer);
    }
}

void CDexSync::getOfferAndSaveInDb(CDataStream &vRecv)
{
    status = Sync;
    CDexOffer offer;
    vRecv >> offer;

    LogPrint("dex", "DEXSYNCOFFER -- get offer info with hash = %s\n", offer.hash.GetHex().c_str());

    if (offer.Check(true)) {
        CDex dex(offer);
        std::string error;
        if (dex.CheckOfferTx(error)) {
            if (offer.isBuy()) {
                if (db->isExistOfferBuy(offer.idTransaction)) {
                    OfferInfo existOffer = db->getOfferBuy(offer.idTransaction);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferBuy(offer);
                    }
                } else {
                    db->addOfferBuy(offer);
                }

                eraseItemFromOffersNeedDownload(offer.hash);
            } else if (offer.isSell())  {
                if (db->isExistOfferSell(offer.idTransaction)) {
                    OfferInfo existOffer = db->getOfferSell(offer.idTransaction);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferSell(offer);
                    }
                } else {
                    db->addOfferSell(offer);
                }

                eraseItemFromOffersNeedDownload(offer.hash);
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

            eraseItemFromOffersNeedDownload(offer.hash);
        }
        if (DexDB::bOffersRescan && !db->isExistMyOffer(offer.idTransaction)) {
            CPubKey kPubKeyObj = offer.getPubKeyObject();
            if (kPubKeyObj.IsValid()) {
                if (pwalletMain->HaveKey(kPubKeyObj.GetID())) {
                    MyOfferInfo mOfferInfo = offer;
                    db->addMyOffer(mOfferInfo);
                }
            }
        }
    }
}

void CDexSync::insertItemFromOffersNeedDownload(const uint256 &hash)
{
    auto pair = offersNeedDownload.insert(hash);

    if (pair.second) {
        maxOffersNeedDownload++;
    }
}

void CDexSync::eraseItemFromOffersNeedDownload(const uint256 &hash)
{
    auto it = offersNeedDownload.find(hash);

    if (it != offersNeedDownload.end()) {
        offersNeedDownload.erase(it);
    }

    float percent = 1 - static_cast<float>(offersNeedDownload.size()) / maxOffersNeedDownload;

    if (offersNeedDownload.size() == 0) {
        finishSyncDex();
    } else {
        uiInterface.NotifyAdditionalDataSyncProgressChanged(percent);
    }
}

bool CDexSync::canStart()
{
    auto vNodesCopy = CopyNodeVector();
    int nDex = 0;
    for (auto pNode : vNodesCopy) {
        if (!pNode->fInbound && !pNode->fMasternode) {
            if (pNode->nVersion >= MIN_DEX_VERSION && mnodeman.isExist(pNode)) {
                nDex++;
            }
        }
    }

    ReleaseNodeVector(vNodesCopy);

    if (nDex >= minNumDexNode()) {
        return true;
    }

    return false;
}

void DexConnectSignals()
{
    masternodeSync.syncFinished.connect(boost::bind(&CDexSync::startSyncDex, &dexsync));
    dexman.startSyncDex.connect(boost::bind(&CDexSync::startSyncDex, &dexsync));
}


void FinishSyncDex()
{
    if (dexsync.statusSync() == CDexSync::Initial) {
        dexsync.finishSyncDex();
    }
}

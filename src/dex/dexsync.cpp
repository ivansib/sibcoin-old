#include <set>
#include <cmath>
#include "dexsync.h"
#include "dexmanager.h"
#include "masternode-sync.h"
#include "masternodeman.h"
#include "init.h"
#include "ui_interface.h"
#include "pubkey.h"
#include "wallet.h"

namespace dex {

CDexSync dexsync;

const int MIN_NUMBER_DEX_NODE = 4;
const int MIN_NUMBER_DEX_NODE_TESTNET = 2;
const int PART_SIZE = 100;

CDexSync::CDexSync()
{
    status = NoStarted;
    statusPercent = 0;
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
    } else if (strCommand == NetMsgType::DEXSYNCPARTHASH) {
        getHashs(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCGETOFFER) {
        sendOffer(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCOFFER) {
        getOfferAndSaveInDb(pfrom, vRecv);
    }
}

void CDexSync::startSyncDex()
{
    if (status == NoStarted) {
        uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent);
    }

    if (!canStart() || status != NoStarted) {
        return;
    }

    status = Started;

    prevMaxOffersNeedDownload = 0;
    prevOffersNeedDownloadSize = 0;

    uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent + 0.01);

    LogPrint("dex", "CDexSync -- start synchronization offers\n");
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
    uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent + 0.1);

    ReleaseNodeVector(vNodesCopy);

    startTimer();
}

void CDexSync::finishSyncDex()
{
    LogPrint("dex", "CDexSync -- finish sync\n");
    status = Finished;
    uiInterface.NotifyAdditionalDataSyncProgressChanged(1);
    syncFinished();
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
        str = _("Synchronization offers doesn't start...");
        break;
    case Started:
        str = _("Synchronization offers started...");
        break;
    case Initial:
        str = _("Synchronization offers pending...");
        break;
    case SyncStepOne:
    case SyncStepSecond:
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

bool CDexSync::reset()
{
    if (status != Finished && status != NoStarted) {
        return false;
    } else {
        statusPercent = 0;
        status = NoStarted;
        startSyncDex();
    }
    
    return true;
}

void CDexSync::updatePrevData()
{
    prevOffersNeedDownloadSize = offersNeedDownload.size();
    prevMaxOffersNeedDownload = maxOffersNeedDownload;
}

bool CDexSync::checkSyncData()
{
    if (prevOffersNeedDownloadSize == offersNeedDownload.size() && prevMaxOffersNeedDownload == maxOffersNeedDownload) {
        return false;
    }

    return true;
}

void CDexSync::startTimer() const
{
    Timer timer(30000, FinishSyncDex);
}

int CDexSync::offersNeedDownloadSize() const
{
    return offersNeedDownload.size();
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

    int maxPart = std::ceil(static_cast<float>(hvs.size())/PART_SIZE);
    int cPart = 1;

    while (hvs.size() > 0) {
        std::list<std::pair<uint256, int>> subHvs;
        auto end = hvs.begin();

        if (hvs.size() > PART_SIZE) {
            std::advance(end, PART_SIZE);
        } else {
            end = hvs.end();
        }

        if (cPart +1 == maxPart) {
            int f = 0;
        }

        subHvs.splice(subHvs.begin(), hvs, hvs.begin(), end);

        LogPrint("dex", "DEXSYNCGETALLHASH -- send list pairs hashe and version\n");
        pfrom->PushMessage(NetMsgType::DEXSYNCPARTHASH, subHvs, cPart, maxPart);
        cPart++;
    }
}

void CDexSync::getHashs(CNode *pfrom, CDataStream &vRecv)
{
    if (status == Initial) {
        status = SyncStepOne;
    }
    std::list<std::pair<uint256, int>> nodeHvs;
    int cPart;
    int maxPart;
    vRecv >> nodeHvs;
    vRecv >> cPart;
    vRecv >> maxPart;
    auto hvs = dexman.availableOfferHashAndVersion();

    for (auto h : nodeHvs) {
        auto found = std::find_if(hvs.begin(), hvs.end(), [h](std::pair<uint256, int> item){ return item.first == h.first; });

        auto isFound = false;

        if (found != hvs.end()) {
            if (h.second > found->second) {
                isFound = true;
            }
        } else {
            isFound = true;
        }

        if (!isFound) {
            continue;
        }

        offersNeedDownload.insert(h.first);
    }

    if (cPart == maxPart) {
        maxOffersNeedDownload = offersNeedDownload.size();
        status = SyncStepSecond;
        sendRequestForGetOffers();
    }
}

void CDexSync::sendOffer(CNode *pfrom, CDataStream &vRecv) const
{
    LogPrint("dex", "DEXSYNCGETOFFER -- receive request on send offer from %s\n", pfrom->addr.ToString());

    uint256 hash;
    vRecv >> hash;

    auto offer = dexman.getOfferInfo(hash);

    if (!offer.IsNull()) {
        LogPrint("dex", "DEXSYNCGETOFFER -- send offer info with hash = %s\n", hash.GetHex().c_str());
        pfrom->PushMessage(NetMsgType::DEXSYNCOFFER, offer);
    } else {
        LogPrint("dex", "DEXSYNCGETOFFER -- offer with hash = %s not found\n", hash.GetHex().c_str());
        Misbehaving(pfrom->GetId(), 1);
    }
}

void CDexSync::getOfferAndSaveInDb(CNode* pfrom, CDataStream &vRecv)
{
    CDexOffer offer;
    vRecv >> offer;

    LogPrint("dex", "DEXSYNCOFFER -- get offer info with hash = %s\n", offer.hash.GetHex().c_str());

    if (offer.Check(true)) {
        CDex dex(offer);
        std::string error;
        if (dex.CheckOfferTx(error)) {
            if (offer.isBuy()) {
                if (db->isExistOfferBuyByHash(offer.hash)) {
                    OfferInfo existOffer = db->getOfferBuyByHash(offer.hash);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferBuy(offer, false);
                    }
                } else {
                    db->addOfferBuy(offer, false);
                }
            } else if (offer.isSell())  {
                if (db->isExistOfferSellByHash(offer.hash)) {
                    OfferInfo existOffer = db->getOfferSellByHash(offer.hash);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferSell(offer, false);
                    }
                } else {
                    db->addOfferSell(offer, false);
                }
            }
        } else {
            dexman.getUncOffers()->updateOffer(offer);
        }

        if (DexDB::bOffersRescan && !db->isExistMyOfferByHash(offer.hash)) {
            CPubKey kPubKeyObj = offer.getPubKeyObject();
            if (kPubKeyObj.IsValid()) {
                if (pwalletMain->HaveKey(kPubKeyObj.GetID())) {
                    MyOfferInfo mOfferInfo = offer;

                    if (dex.CheckOfferTx(error)) {
                        mOfferInfo.status = Active;
                        db->addMyOffer(mOfferInfo, false);
                    }
                }
            }
        }
    } else {
        LogPrint("dex", "DEXSYNCOFFER -- offer check fail\n");
        Misbehaving(pfrom->GetId(), 20);
    }

    eraseItemFromOffersNeedDownload(offer.hash);
}

void CDexSync::eraseItemFromOffersNeedDownload(const uint256 &hash)
{
    auto it = offersNeedDownload.find(hash);

    if (it != offersNeedDownload.end()) {
        offersNeedDownload.erase(it);
    }

    float p = static_cast<float>(offersNeedDownload.size()) / maxOffersNeedDownload;
    float percent = 1 - 0.9 * static_cast<float>(offersNeedDownload.size()) / maxOffersNeedDownload;

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

void CDexSync::sendRequestForGetOffers() const
{
    auto vNodesCopy = CopyNodeVector();

    int iNode = 0;
    int i = 0;
    bool isSend = true;

    auto it = offersNeedDownload.begin();
    while (it != offersNeedDownload.end()) {
        auto node = vNodesCopy[iNode];

        if (iNode == vNodesCopy.size() - 1) {
            iNode = 0;
        } else {
            iNode++;
        }

        if (node->nVersion < MIN_DEX_VERSION) {
            continue;
        }

        if (!mnodeman.isExist(node)) {
            continue;
        }

        if(node->fMasternode || (fMasterNode && node->fInbound)) {
            continue;
        }

         auto i = *it;
         node->PushMessage(NetMsgType::DEXSYNCGETOFFER, *it);
         ++it;
    }

    ReleaseNodeVector(vNodesCopy);
    startTimer();
}

void DexConnectSignals()
{
    masternodeSync.syncFinished.connect(boost::bind(&CDexSync::startSyncDex, &dexsync));
    dexman.startSyncDex.connect(boost::bind(&CDexSync::startSyncDex, &dexsync));
}


void FinishSyncDex()
{
    if (dexsync.statusSync() == CDexSync::Initial && dexsync.offersNeedDownloadSize() == 0) {
        dexsync.reset();
    } else if (dexsync.statusSync() == CDexSync::SyncStepOne) {
        dexsync.startTimer();
    } else {
        if (!dexsync.checkSyncData()) {
            dexsync.sendRequestForGetOffers();
        } else if (!dexsync.isSynced()) {
            LogPrint("dex", "CDexSync -- restart timer\n");
            dexsync.updatePrevData();
            dexsync.startTimer();
        }
    }
}

}

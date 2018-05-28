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
const int MIN_NUMBER_DEX_NODE_TESTNET = 1;//2;
const int PART_SIZE = 100;

CDexSync::CDexSync()
{
    status = Status::NoStarted;
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
        sendHashOffers(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCPARTHASH) {
        getHashs(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCGETOFFER) {
        sendOffer(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCOFFER) {
        getOfferAndSaveInDb(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCNOOFFERS) {
        noOffersList(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXSYNCNOHASH) {
        noHash(pfrom, vRecv);
    }
}

void CDexSync::startSyncDex()
{
    if (status == Status::NoStarted) {
        uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent);
    }

    if (!canStart() || status != Status::NoStarted) {
        return;
    }

    status = Status::Started;

    prevMaxOffersNeedDownload = 0;
    prevOffersNeedDownloadSize = 0;

    uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent + 0.01);

    LogPrint("dex", "CDexSync -- start synchronization offers\n");
    maxOffersNeedDownload = 0;
    auto vNodesCopy = CopyNodeVector();

    initSetWaitAnswerFromNodes(vNodesCopy);

    DexSyncInfo dsInfo = dexSyncInfo();

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

        node->PushMessage(NetMsgType::DEXSYNCGETALLHASH, dsInfo);
    }

    status = Status::Initial;
    uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent + 0.1);

    ReleaseNodeVector(vNodesCopy);

    startTimer();
    startTimerForAnswer();
}

void CDexSync::finishSyncDex()
{
    LogPrint("dex", "CDexSync -- finish sync\n");
    status = Status::Finished;
    uiInterface.NotifyAdditionalDataSyncProgressChanged(1);
    syncFinished();
}

bool CDexSync::isSynced() const
{
    return status == Status::Finished;
}

std::string CDexSync::getSyncStatus() const
{
    std::string str;
    switch (status) {
    case Status::NoStarted:
        str = _("Synchronization offers doesn't start...");
        break;
    case Status::Started:
        str = _("Synchronization offers started...");
        break;
    case Status::Initial:
        str = _("Synchronization offers pending...");
        break;
    case Status::SyncStepOne:
    case Status::SyncStepSecond:
        str = _("Synchronization offers...");
        break;
    case Status::Finished:
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
    if (status == Status::SyncStepOne || status == Status::SyncStepSecond) {
        return false;
    } else {
        statusPercent = 0;
        status = Status::NoStarted;
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

void CDexSync::startTimerForAnswer() const
{
    Timer timer(30000, StopTimerForAnswer);
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

void CDexSync::sendHashOffers(CNode *pfrom, CDataStream &vRecv) const
{
    LogPrint("dex", "DEXSYNCGETALLHASH -- receive request on send list pairs hashe and version from %s\n", pfrom->addr.ToString());

    DexSyncInfo dsInfo = dexSyncInfo();
    DexSyncInfo dsInfoOther;
    vRecv >> dsInfoOther;

    if (dsInfoOther == dsInfo && dsInfo != DexSyncInfo()) {
        LogPrint("dex", "DEXSYNCGETALLHASH -- offers actual\n");
        pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::Actual));

        return;
    }

    std::list<std::pair<uint256, uint32_t>> hvs;

    if (dsInfoOther == DexSyncInfo()) {
        hvs = dexman.availableOfferHashAndVersion();
    } else {
        hvs = dexman.availableOfferHashAndVersion(dsInfoOther.lastTimeMod);
    }

    int maxPart = std::ceil(static_cast<float>(hvs.size())/PART_SIZE);
    int cPart = 1;

    if (hvs.size() == 0) {
        LogPrint("dex", "DEXSYNCGETALLHASH -- offers not found\n");
        pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::Empty));
    }

    while (hvs.size() > 0) {
        std::list<std::pair<uint256, uint32_t>> subHvs;
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

        LogPrint("dex", "DEXSYNCGETALLHASH -- send list pairs of hash and version\n");
        pfrom->PushMessage(NetMsgType::DEXSYNCPARTHASH, subHvs, cPart, maxPart);
        cPart++;
    }
}

void CDexSync::getHashs(CNode *pfrom, CDataStream &vRecv)
{
    addAddrToStatusNode(pfrom->addr, StatusNode::Process);

    if (status == Status::Initial) {
        status = Status::SyncStepOne;
    }
    std::list<std::pair<uint256, uint32_t>> nodeHvs;
    int cPart;
    int maxPart;
    vRecv >> nodeHvs;
    vRecv >> cPart;
    vRecv >> maxPart;
    auto hvs = dexman.availableOfferHashAndVersion();

    for (auto h : nodeHvs) {
        auto found = std::find_if(hvs.begin(), hvs.end(), [h](std::pair<uint256, uint32_t> item){ return item.first == h.first; });

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
        status = Status::SyncStepSecond;
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
        pfrom->PushMessage(NetMsgType::DEXSYNCNOHASH, hash);
    }
}

void CDexSync::getOfferAndSaveInDb(CNode* pfrom, CDataStream &vRecv)
{
    addAddrToStatusNode(pfrom->addr, StatusNode::Process);

    CDexOffer offer;
    vRecv >> offer;

    LogPrint("dex", "DEXSYNCOFFER -- get offer info with hash = %s\n", offer.hash.GetHex().c_str());

    int fine = 0;
    if (offer.Check(true, fine)) {
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
        LogPrint("DEXSYNCOFFER -- offer check fail, hash: %s\n", offer.hash.GetHex().c_str());
        Misbehaving(pfrom->GetId(), fine);
    }

    eraseItemFromOffersNeedDownload(offer.hash);
}

void CDexSync::noOffersList(CNode *pfrom, CDataStream &vRecv)
{
    StatusOffers status;
    int s;
    vRecv >> s;
    status = static_cast<StatusOffers>(s);
    if (status == StatusOffers::Actual) {
        addAddrToStatusNode(pfrom->addr, StatusNode::Actual);
    } else {
        addAddrToStatusNode(pfrom->addr, StatusNode::Bad);
    }
}

void CDexSync::noHash(CNode *pfrom, CDataStream &vRecv)
{
    addAddrToStatusNode(pfrom->addr, StatusNode::Process);
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

void CDexSync::initSetWaitAnswerFromNodes(const std::vector<CNode *> &nodes)
{
    waitAnswerFromNodes.clear();

    for (auto node : nodes) {
        waitAnswerFromNodes.insert(node->addr);
    }
}

void CDexSync::addAddrToStatusNode(const CAddress &addr, StatusNode status)
{
    auto it = waitAnswerFromNodes.find(addr);

    if (it != waitAnswerFromNodes.end()) {
        waitAnswerFromNodes.erase(it);
    }

    statusNodes[addr] = status;
}

DexSyncInfo CDexSync::dexSyncInfo() const
{
    DexSyncInfo ds;
    ds.checkSum = 0;
    ds.count = db->countOffersSell() + db->countOffersBuy();

    uint64_t lastModSell = db->lastModificationOffersSell();
    uint64_t lastModBuy = db->lastModificationOffersBuy();
    if (lastModBuy > lastModSell) {
        ds.lastTimeMod = lastModBuy;
    } else {
        ds.lastTimeMod = lastModSell;
    }

    return ds;
}

std::set<uint256> CDexSync::getOffersNeedDownload() const
{
    return offersNeedDownload;
}

void CDexSync::setOffersNeedDownload(const std::set<uint256> &value)
{
    offersNeedDownload = value;
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

        if (statusNodes.at(node->addr) == StatusNode::Bad || statusNodes.at(node->addr) == StatusNode::Actual) {
            continue;
        }

        if (node->nVersion < MIN_DEX_VERSION) {
            continue;
        }

        if (!mnodeman.isExist(node)) {
            continue;
        }

        if(node->fMasternode || (fMasterNode && node->fInbound)) {
            ++it;
            continue;
        }

        node->PushMessage(NetMsgType::DEXSYNCGETOFFER, *it);
        ++it;
    }

    ReleaseNodeVector(vNodesCopy);
    startTimer();
}

void CDexSync::checkNodes()
{
    int numActual = 0;
    int numProcess = 0;

    auto it = statusNodes.begin();

    while (it != statusNodes.end()) {
        if (it->second == StatusNode::Actual) {
            numActual++;
        } else if (it->second == StatusNode::Process) {
            numProcess++;
        }

        ++it;
    }

    if (numActual > 0 && numProcess == 0) {
        finishSyncDex();
        statusNodes.clear();
    } else {
        auto it = waitAnswerFromNodes.begin();
        while (it != waitAnswerFromNodes.end()) {
            statusNodes[*it] = StatusNode::Bad;
            ++it;
        }
    }

    waitAnswerFromNodes.clear();
}

void DexConnectSignals()
{
    masternodeSync.syncFinished.connect(boost::bind(&CDexSync::startSyncDex, &dexsync));
    dexman.startSyncDex.connect(boost::bind(&CDexSync::startSyncDex, &dexsync));
}


void FinishSyncDex()
{
    if (dexsync.statusSync() == CDexSync::Status::Initial && dexsync.offersNeedDownloadSize() == 0) {
        dexsync.reset();
    } else if (dexsync.statusSync() == CDexSync::Status::SyncStepOne) {
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

void StopTimerForAnswer()
{
    dexsync.checkNodes();
}

}

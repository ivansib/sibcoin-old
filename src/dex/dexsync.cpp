#include <set>
#include <cmath>
#include "dexsync.h"
#include "dexmanager.h"
#include "dexcrc.h"
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

const int MAX_STEP_WAIT_AFTER_FAILED = 30;
const int MAX_NUM_ATTEMPT_START = 10;
const int MAX_NUM_AUTO_RESET = 5;

CDexSync::CDexSync()
{
    status = Status::NoStarted;
    statusPercent = 0;
    db = nullptr;
    isRunTimer = false;

    numUnanswerRequests = 0;
    stepWaitAfterFailed = 0;
    numAttemptStart = 0;
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
        sendHashOffers(pfrom, vRecv, false);
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
    } else if (strCommand == NetMsgType::DEXSYNCNEEDSYNC) {
        reset();
    } else if (strCommand == NetMsgType::DEXSYNCCHECK) {
         sendHashOffers(pfrom, vRecv, true);
    }
}

void CDexSync::startSyncDex()
{
    initDB();

    if (status == Status::NoStarted) {
        uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent);
    }

    if (!canStart() || status != Status::NoStarted) {
        numAttemptStart++;

        if (numAttemptStart >= MAX_NUM_ATTEMPT_START) {
            status = Status::Failed;
            stepWaitAfterFailed = 0;
            uiInterface.NotifyAdditionalDataSyncProgressChanged(0);
        }

        return;
    }

    numAttemptStart = 0;
    status = Status::Started;

    clearData();

    LogPrint("dex", "CDexSync -- start synchronization offers\n");

    uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent + 0.01);

    auto vNodesCopy = CopyNodeVector();

    initSetWaitAnswerFromNodes(vNodesCopy);

    DexSyncInfo dsInfo = dexSyncInfo(lastModOffers - 3600);

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
    if (actualSync()) {
        prevMaxOffersNeedDownload = 0;
        prevOffersNeedDownloadSize = 0;
        maxOffersNeedDownload = 0;

        statusNodes.clear();
        waitAnswerFromNodes.clear();

        LogPrint("dex", "CDexSync -- finish sync\n");
        status = Status::Finished;
        uiInterface.NotifyAdditionalDataSyncProgressChanged(1);
        syncFinished();
    } else {
        status = Status::NoStarted;
        reset(true);
    }
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
    case Status::Failed:
        str = _("Synchronization offers failed");
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

bool CDexSync::reset(const bool isAuto)
{
    if (isAuto) {
        numAutoReset++;

        if (numAutoReset > MAX_NUM_AUTO_RESET) {
            status = Status::Failed;
            numAutoReset = 0;
            numUnanswerRequests = 0;
            stepWaitAfterFailed = 0;

            uiInterface.NotifyAdditionalDataSyncProgressChanged(0);

            return false;
        }
    }

    bool b = (status == Status::SyncStepOne || status == Status::SyncStepSecond);
    bool b1 = (numUnanswerRequests < 3);

    if (b && b1) {
        return false;
    } else {        
        statusPercent = 0;
        numUnanswerRequests = 0;
        status = Status::NoStarted;
        startSyncDex();
    }
    
    return true;
}

bool CDexSync::resetAfterFailed()
{
    if (status == Status::Failed) {
        stepWaitAfterFailed++;

        if (stepWaitAfterFailed >= MAX_STEP_WAIT_AFTER_FAILED) {
            statusPercent = 0;
            status = Status::NoStarted;
            startSyncDex();

            return true;
        }
    }

    return false;
}

void CDexSync::forceSynced()
{
    status = Status::Finished;
    uiInterface.NotifyAdditionalDataSyncProgressChanged(1);
    clearData();
}

void CDexSync::updatePrevData()
{
    numUnanswerRequests = 0;
    prevOffersNeedDownloadSize = offersNeedDownload.size();
    prevMaxOffersNeedDownload = maxOffersNeedDownload;
}

bool CDexSync::checkSyncData()
{
    if (prevOffersNeedDownloadSize == offersNeedDownload.size() && prevMaxOffersNeedDownload == maxOffersNeedDownload) {
        numUnanswerRequests++;
        return false;
    }

    return true;
}

void CDexSync::startTimer()
{
    if (!isRunTimer) {
        Timer timer(30000, FinishSyncDex);
        isRunTimer = true;
    }
}

void CDexSync::setRunTimer(const bool b)
{
    isRunTimer = b;
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
        lastModOffers = lastTimeModOffers();
        timeStart = GetAdjustedTime();
    }
}

void CDexSync::sendHashOffers(CNode *pfrom, CDataStream &vRecv, bool isCheck) const
{
    std::string tag = "DEXSYNCGETALLHASH";
    if (isCheck) {
        tag = "DEXSYNCCHECK";
    }
    LogPrint("dex", "%s -- receive request on send list pairs hashe and version from %s\n", tag, pfrom->addr.ToString());

    if (!isSynced()) {
        LogPrint("dex", "%s -- offers no synchonized\n", tag);
        pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::NoSync));
        return;
    }

    DexSyncInfo dsInfoOther;
    vRecv >> dsInfoOther;

    std::list<std::pair<uint256, uint32_t>> hvs;

    if (dsInfoOther.isNull()) {
        hvs = dexman.availableOfferHashAndVersion();
    } else {
        DexSyncInfo dsInfo = dexSyncInfo(dsInfoOther.lastTimeMod);

        if (isCheck) {
            if (dsInfoOther == dsInfo) {
                LogPrint("dex", "%s -- offers actual\n", tag);
                pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::Actual));
                return;
            }
        } else {
        if (dsInfoOther == dsInfo) {
            hvs = dexman.availableOfferHashAndVersion(DexDB::OffersPeriod::After, dsInfoOther.lastTimeMod);
            if (hvs.size() == 0) {
                LogPrint("dex", "%s -- offers actual\n", tag);
                pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::Actual));
                return;
            }
        } else {
            hvs = dexman.availableOfferHashAndVersion();
        }
        }
    }

    int maxPart = std::ceil(static_cast<float>(hvs.size())/PART_SIZE);
    int cPart = 1;

    if (hvs.size() == 0) {
        if (isCheck) {
            LogPrint("dex", "%s -- offers actual\n", tag);
            pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::Actual));
            return;
        } else {
        LogPrint("dex", "%s -- offers not found\n", tag);
        pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::Empty));
        }
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

        LogPrint("dex", "%s -- send list pairs of hash and version\n", tag);
        pfrom->PushMessage(NetMsgType::DEXSYNCPARTHASH, subHvs, cPart, maxPart);
        cPart++;
    }
}

void CDexSync::getHashs(CNode *pfrom, CDataStream &vRecv)
{
    addAddrToStatusNode(pfrom->addr, StatusNode::Good);

    if (status == Status::Initial) {
        status = Status::SyncStepOne;
        uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent);
    }
    std::list<std::pair<uint256, uint32_t>> nodeHvs;
    int cPart;
    int maxPart;
    vRecv >> nodeHvs;
    vRecv >> cPart;
    vRecv >> maxPart;
    auto hvs = dexman.availableOfferHashAndVersion();

    LOCK2(cs_main, cs);

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
        addAddrToStatusNode(pfrom->addr, StatusNode::Process, false);
    }

    if (cPart == maxPart) {
        if (statusNodes[pfrom->addr] == StatusNode::Good) { // WARNING: edit it (sends messages to nodes that are already synchronized)
            LogPrint("dex", "DEXSYNCPARTHASH -- send message about node with addres %s need sync\n", pfrom->addr.ToString());
            pfrom->PushMessage(NetMsgType::DEXSYNCNEEDSYNC);
        }

        maxOffersNeedDownload = offersNeedDownload.size();
        status = Status::SyncStepSecond;
        uiInterface.NotifyAdditionalDataSyncProgressChanged(statusPercent);
        sendRequestNodes();
    }
}

void CDexSync::sendOffer(CNode *pfrom, CDataStream &vRecv) const
{
    LogPrint("dex", "DEXSYNCGETOFFER -- receive request on send offer from %s\n", pfrom->addr.ToString());

    if (!isSynced()) {
        LogPrint("dex", "DEXSYNCGETOFFER -- offers no synchonized\n");
        pfrom->PushMessage(NetMsgType::DEXSYNCNOOFFERS, static_cast<int>(StatusOffers::NoSync));
        return;
    }

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
    addAddrToStatusNode(pfrom->addr, StatusNode::Good);

    CDexOffer offer;
    vRecv >> offer;

    LogPrint("dex", "DEXSYNCOFFER -- get offer info with hash = %s\n", offer.hash.GetHex().c_str());

    int fine = 0;
    auto hash = offer.hash;
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
        LogPrint("DEXSYNCOFFER -- offer check fail, hash: %s\n", hash.GetHex().c_str());
        Misbehaving(pfrom->GetId(), fine);
    }

    eraseItemFromOffersNeedDownload(hash);
}

void CDexSync::noOffersList(CNode *pfrom, CDataStream &vRecv)
{
    StatusOffers status;
    int s;
    vRecv >> s;
    status = static_cast<StatusOffers>(s);
    if (status == StatusOffers::NoSync) {
        addAddrToStatusNode(pfrom->addr, StatusNode::Bad);
    } else {
        addAddrToStatusNode(pfrom->addr, StatusNode::Actual);
    }
}

void CDexSync::noHash(CNode *pfrom, CDataStream &vRecv)
{
    addAddrToStatusNode(pfrom->addr, StatusNode::Good);
}

void CDexSync::eraseItemFromOffersNeedDownload(const uint256 &hash)
{
    LOCK2(cs_main, cs);

    auto it = offersNeedDownload.find(hash);

    if (it != offersNeedDownload.end()) {
        offersNeedDownload.erase(it);
    }

    float percent = 1 - 0.9 * static_cast<float>(offersNeedDownload.size()) / maxOffersNeedDownload;
    statusPercent = percent;

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

void CDexSync::addAddrToStatusNode(const CAddress &addr, StatusNode status, const bool isLock)
{
    if (isLock) {
        LOCK2(cs_main, cs);
    }

    auto it = waitAnswerFromNodes.find(addr);

    if (it != waitAnswerFromNodes.end()) {
        waitAnswerFromNodes.erase(it);
    }

    if (!(statusNodes[addr] == StatusNode::Process && status == StatusNode::Good)) {
        statusNodes[addr] = status;
    }
}

DexSyncInfo CDexSync::dexSyncInfo(const uint64_t &lastMod) const
{
    DexSyncInfo ds;

    db->begin();
    auto buyOffers = db->getHashsAndEditingVersionsBuy(DexDB::OffersPeriod::Before, lastMod);
    auto sellOffers = db->getHashsAndEditingVersionsSell(DexDB::OffersPeriod::Before, lastMod);
    db->commit();

    CDexCrc crc(buyOffers);
    crc += sellOffers;

    ds.checkSum = crc;
    ds.count = buyOffers.size() + sellOffers.size();
    ds.lastTimeMod = lastMod;

    return ds;
}

uint64_t CDexSync::lastTimeModOffers() const
{
    uint64_t time;

    db->begin();
    auto lastModSell = db->lastModificationOffersSell();
    auto lastModBuy = db->lastModificationOffersBuy();
    db->commit();

    if (lastModBuy > lastModSell) {
        time = lastModBuy;
    } else {
        time = lastModSell;
    }

    return time;
}

void CDexSync::sendCheckNodes() const
{
    auto vNodesCopy = CopyNodeVector();

    DexSyncInfo dsInfo = dexSyncInfo(timeStart);
    for (auto node : vNodesCopy) {
        auto sn = statusNodes.find(node->addr);
        if (sn != statusNodes.end()) {
            if (sn->second == StatusNode::Good) {
                node->PushMessage(NetMsgType::DEXSYNCCHECK, dsInfo);
            }
        }
    }

    ReleaseNodeVector(vNodesCopy);
}

void CDexSync::sendRequestFoGetOffers() const
{
    auto vNodesCopy = CopyNodeVector();

    int iNode = 0;
    int i = 0;
    bool isSend = true;

    bool interviewAll = false;

    if (offersNeedDownload.size() < vNodesCopy.size() * 2) {
        interviewAll = true;
    }
    auto it = offersNeedDownload.begin();
    while (it != offersNeedDownload.end()) {
        auto node = vNodesCopy[iNode];

        if (iNode == vNodesCopy.size() - 1) {
            iNode = 0;
        } else {
            iNode++;
        }

        auto hash = *it;

        if (interviewAll) {
            if (iNode == 0) {
                ++it;
            }
        } else {
            ++it;
        }

        auto sn = statusNodes.find(node->addr);
        if (sn != statusNodes.end()) {
            if (sn->second == StatusNode::Bad || sn->second == StatusNode::Actual) {
                continue;
            }
        } else {
            DexSyncInfo dsInfo = dexSyncInfo(lastModOffers - 3600);
            node->PushMessage(NetMsgType::DEXSYNCGETALLHASH, dsInfo);
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

        node->PushMessage(NetMsgType::DEXSYNCGETOFFER, hash);
    }

    ReleaseNodeVector(vNodesCopy);
}

bool CDexSync::actualSync() const
{
    int numActual = 0;
    int numProcess = 0;

    auto it = statusNodes.begin();

    while (it != statusNodes.end()) {
        if (it->second == StatusNode::Actual) {
            numActual++;
        } else if (it->second == StatusNode::Good || it->second == StatusNode::Process) {
            numProcess++;
        }

        ++it;
    }

    if (numActual > 0 && numProcess == 0) {
        return true;
    }

    return false;
}

int CDexSync::numberUnanswerRequests() const
{
    return numUnanswerRequests;
}

void CDexSync::clearData()
{
    prevMaxOffersNeedDownload = 0;
    prevOffersNeedDownloadSize = 0;
    maxOffersNeedDownload = 0;
    numUnanswerRequests = 0;

    LOCK2(cs_main, cs);

    statusNodes.clear();
    waitAnswerFromNodes.clear();
    offersNeedDownload.clear();
}

std::set<uint256> CDexSync::getOffersNeedDownload() const
{
    return offersNeedDownload;
}

void CDexSync::sendRequestNodes()
{
    if (offersNeedDownload.size() == 0 && !actualSync()) {
        sendCheckNodes();
    } else {
        sendRequestFoGetOffers();
    }

    startTimer();
}

void CDexSync::checkNodes()
{
    LOCK2(cs_main, cs);

    if (actualSync()) {
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
    dexsync.setRunTimer(false);

    if (dexsync.statusSync() == CDexSync::Status::Finished) {
        dexsync.clearData();
        return;
    }

    bool b = (dexsync.statusSync() == CDexSync::Status::Initial && dexsync.offersNeedDownloadSize() == 0);
    bool b1 = (dexsync.numberUnanswerRequests() >= 3);
    if (b || b1) {
        dexsync.reset(true);
    } else {
        LOCK2(cs_main, dexsync.cs);

        if (dexsync.actualSync()) {
            dexsync.finishSyncDex();
        } else if (dexsync.statusSync() == CDexSync::Status::SyncStepOne) {
            dexsync.startTimer();
        } else {
            if (!dexsync.checkSyncData()) {
                dexsync.sendRequestNodes();
            } else if (!dexsync.isSynced()) {
                LogPrint("dex", "CDexSync -- restart timer\n");
                dexsync.updatePrevData();
                dexsync.startTimer();
            }
        }
    }
}

void StopTimerForAnswer()
{
    dexsync.checkNodes();
}

}

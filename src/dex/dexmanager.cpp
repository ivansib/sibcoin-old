
#include "dexmanager.h"

#include "init.h"
#include "wallet.h"
#include "util.h"
#include "utilstrencodings.h"
#include "masternodeman.h"
#include "masternode-sync.h"

#include "dex/db/dexdb.h"
#include "dexsync.h"
#include "txmempool.h"
#include "base58.h"



CDexManager dexman;


CDexManager::CDexManager()
{
    db = nullptr;
    uncOffers = new UnconfirmedOffers();
}

CDexManager::~CDexManager()
{
    if (db != nullptr) {
        db->freeInstance();
    }

    delete uncOffers;
}

void CDexManager::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    initDB();

    if (strCommand == NetMsgType::DEXOFFBCST) {
        getAndSendNewOffer(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXDELOFFER) {
        getAndDelOffer(pfrom, vRecv);
    } else if (strCommand == NetMsgType::DEXOFFEDIT) {
        getAndSendEditedOffer(pfrom, vRecv);
    }
}

void CDexManager::addOrEditDraftMyOffer(MyOfferInfo &myOffer)
{
    initDB();

    myOffer.status = Draft;

    CDexOffer dexOffer;
    uint256 oldHash = myOffer.hash;
    dexOffer.Create(myOffer);

    if (!oldHash.IsNull() && oldHash != dexOffer.hash) {
        db->deleteMyOfferByHash(oldHash);
    }

    myOffer.setOfferInfo(dexOffer);

    saveMyOffer(myOffer);
}

void CDexManager::prepareAndSendMyOffer(MyOfferInfo &myOffer, std::string &error)
{
    initDB();

    CDexOffer dexOffer;

    if (myOffer.status == Indefined || myOffer.status == Draft) {
        uint256 oldHash = myOffer.hash;
        dexOffer.Create(myOffer);

        if (oldHash != dexOffer.hash) {
            db->deleteMyOfferByHash(oldHash);
        }
    } else if (myOffer.status == Active) {
        myOffer.editingVersion++;

        dexOffer = CDexOffer(myOffer);
    }

    CPubKey pubKey = dexOffer.getPubKeyObject();
    if (!pwalletMain->HaveKey(pubKey.GetID())) {
        error = "Don't find pub key";
        return;
    }

    CDex dex(dexOffer);

    CKey key;
    if (!dex.FindKey(key, error)) return;
    if (!dex.MakeEditSign(key, error)) return;

    uint256 tx;

    if (!dexOffer.idTransaction.IsNull()) {
        dexman.sendEditedOffer(dex.offer);

        if (dex.offer.isBuy()) {
            db->editOfferBuy(dex.offer);
        }
        if (dex.offer.isSell()) {
            db->editOfferSell(dex.offer);
        }
    } else if (dex.PayForOffer(tx, error)) {
        dexman.sendNewOffer(dex.offer);

        myOffer.setOfferInfo(dex.offer);
        myOffer.status = Active;
        if (dex.offer.isBuy()) {
            db->addOfferBuy(dex.offer);
        }
        if (dex.offer.isSell()) {
            db->addOfferSell(dex.offer);
        }
    }

    if (error.empty()) {
        saveMyOffer(myOffer);
    }
}

void CDexManager::sendNewOffer(const CDexOffer &offer)
{
    auto vNodesCopy = CopyNodeVector();

    for (auto pNode : vNodesCopy) {
        if (pNode->nVersion < MIN_DEX_VERSION) {
            continue;
        }

        pNode->PushMessage(NetMsgType::DEXOFFBCST, offer);
    }

    ReleaseNodeVector(vNodesCopy);
}

void CDexManager::sendEditedOffer(const CDexOffer &offer)
{
    std::vector<unsigned char> vchSign = ParseHex(offer.editsign);

    auto vNodesCopy = CopyNodeVector();

    for (auto pNode : vNodesCopy) {
        if (pNode->nVersion < MIN_DEX_VERSION) {
            continue;
        }

        pNode->PushMessage(NetMsgType::DEXOFFEDIT, offer, vchSign);
    }

    ReleaseNodeVector(vNodesCopy);
}

void CDexManager::checkUncOffers()
{
    auto list = uncOffers->getOffers();

    for (auto offer : list) {
        CDex dex(offer);
        std::string error;
        if (dex.CheckOfferTx(error)) {
            if (offer.isBuy())  {
                if (!db->isExistOfferBuy(offer.idTransaction)) {
                    db->addOfferBuy(offer);
                }
            }

            if (offer.isSell())  {
                if (!db->isExistOfferSell(offer.idTransaction)) {
                    db->addOfferSell(offer);
                }
            }

            uncOffers->deleteOffer(offer.hash);
        }
    }
}

void CDexManager::setStatusExpiredForMyOffers()
{
    auto offers = db->getMyOffers();

    uint64_t currentTime = static_cast<uint64_t>(time(NULL));

    for (auto item : offers) {
        if (item.timeToExpiration < currentTime) {
            item.status = dex::Expired;
            db->editMyOffer(item);
        }
    }
}

void CDexManager::deleteOldUncOffers()
{
    uncOffers->deleteOldOffers();
}

void CDexManager::deleteOldOffers()
{
    db->deleteOldOffersBuy();
    db->deleteOldOffersSell();
}

void CDexManager::initDB()
{
    if (db == nullptr) {
        db = DexDB::instance();
    }
}

void CDexManager::getAndSendNewOffer(CNode *pfrom, CDataStream &vRecv)
{
    LogPrint("dex", "DEXOFFBCST -- get new offer from = %s\n", pfrom->addr.ToString());

    CDexOffer offer;
    vRecv >> offer;
    if (offer.Check(true)) {
        CDex dex(offer);
        std::string error;
        if (dex.CheckOfferTx(error)) {
            bool bFound = false;
            if (offer.isBuy())  {
                if (db->isExistOfferBuy(offer.idTransaction)) {
                  bFound = true;
                } else {
                    db->addOfferBuy(offer);
                }
            }

            if (offer.isSell())  {
                if (db->isExistOfferSell(offer.idTransaction)) {
                  bFound = true;
                } else {
                    db->addOfferSell(offer);
                }
            }

            if (!bFound) { // need to save and relay
                auto vNodesCopy = CopyNodeVector();
                for (auto pNode : vNodesCopy) {
                    if (pNode->nVersion < MIN_DEX_VERSION) {
                        continue;
                    }

                    if (pNode->addr != pfrom->addr) {
                        pNode->PushMessage(NetMsgType::DEXOFFBCST, offer);
                    }
                }

                ReleaseNodeVector(vNodesCopy);
            }
            LogPrint("dex", "DEXOFFBCST --\n%s\nfound %d\n", offer.dump().c_str(), bFound);
        } else {
            if (!uncOffers->isExistOffer(offer.hash)) {
                uncOffers->setOffer(offer);

                auto vNodesCopy = CopyNodeVector();
                for (auto pNode : vNodesCopy) {
                    if (pNode->nVersion < MIN_DEX_VERSION) {
                        continue;
                    }

                    if (pNode->addr != pfrom->addr) {
                        pNode->PushMessage(NetMsgType::DEXOFFBCST, offer);
                    }
                }

                ReleaseNodeVector(vNodesCopy);

                LogPrint("dex", "DEXOFFBCST --check offer tx fail(%s)\n", offer.idTransaction.GetHex().c_str());
            }
        }
    } else {
        LogPrint("dex", "DEXOFFBCST -- offer check fail\n");
    }
}


void CDexManager::getAndDelOffer(CNode *pfrom, CDataStream &vRecv)
{
    LogPrint("dex", "DEXDELOFFER -- receive request on delete offer from = %s\n", pfrom->addr.ToString());

    std::vector<unsigned char> vchSign;
    CDexOffer offer;
    vRecv >> offer;
    vRecv >> vchSign;

    if (offer.Check(true)) {
        CDex dex(offer);
        std::string error;
        if (dex.CheckOfferSign(vchSign, error)) {
            bool bFound = false;
            if (offer.isBuy())  {
                if (db->isExistOfferBuy(offer.idTransaction)) {
                    db->deleteOfferBuy(offer.idTransaction);
                    bFound = true;
                }
            }

            if (offer.isSell())  {
                if (db->isExistOfferSell(offer.idTransaction)) {
                    db->deleteOfferSell(offer.idTransaction);
                    bFound = true;
                }
            }
            if (!bFound) {
                if (uncOffers->isExistOffer(offer.hash)) {
                    bFound = true;
                    uncOffers->deleteOffer(offer.hash);
                }

            }

            if (bFound) { // need to delete and relay
                auto vNodesCopy = CopyNodeVector();
                for (auto pNode : vNodesCopy) {
                    if (pNode->nVersion < MIN_DEX_VERSION) {
                        continue;
                    }

                    if (pNode->addr != pfrom->addr) {
                        pNode->PushMessage(NetMsgType::DEXDELOFFER, offer, vchSign);
                    }
                }

                ReleaseNodeVector(vNodesCopy);
            }
            LogPrint("dex", "DEXDELOFFER --\n%s\nfound %d\n", offer.dump().c_str(), bFound);
        } else {
            LogPrint("dex", "DEXDELOFFER --check offer sign fail(%s)\n", offer.hash.GetHex().c_str());
        }
    } else {
        LogPrint("dex", "DEXDELOFFER -- offer check fail\n");
    }
}



void CDexManager::getAndSendEditedOffer(CNode *pfrom, CDataStream& vRecv)
{
    LogPrint("dex", "DEXOFFEDIT -- receive request on edited offer from = %s\n", pfrom->addr.ToString());

    std::vector<unsigned char> vchSign;
    CDexOffer offer;
    vRecv >> offer;
    vRecv >> vchSign;
    offer.editsign = HexStr(vchSign);
    if (offer.Check(true) && offer.CheckEditSign()) {
        CDex dex(offer);
        std::string error;
        bool isActual = false;
        if (dex.CheckOfferTx(error)) {
            if (offer.isBuy()) {
                if (db->isExistOfferBuy(offer.idTransaction)) {
                    OfferInfo existOffer = db->getOfferBuy(offer.idTransaction);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferBuy(offer);
                        isActual = true;
                    }
                } else {
                    db->addOfferBuy(offer);
                    isActual = true;
                }
            }

            if (offer.isSell()) {
                if (db->isExistOfferSell(offer.idTransaction)) {
                    OfferInfo existOffer = db->getOfferSell(offer.idTransaction);
                    if (offer.editingVersion > existOffer.editingVersion) {
                        db->editOfferSell(offer);
                        isActual = true;
                    }
                } else {
                    db->addOfferSell(offer);
                    isActual = true;
                }
            }
            LogPrint("dex", "DEXOFFEDIT --\n%s\nactual %d\n", offer.dump().c_str(), isActual);
        } else {
            if (uncOffers->isExistOffer(offer.hash)) {
                OfferInfo existOffer = uncOffers->getOffer(offer.hash);
                if (offer.editingVersion > existOffer.editingVersion) {
                    uncOffers->deleteOffer(offer.hash);
                    uncOffers->setOffer(offer);
                    isActual = true;
                }
            } else {
                uncOffers->setOffer(offer);
                isActual = true;
            }
            LogPrint("dex", "DEXOFFEDIT --check offer tx fail(%s)\n", offer.idTransaction.GetHex().c_str());
        }
        if (isActual) {
            auto vNodesCopy = CopyNodeVector();
            for (auto pNode : vNodesCopy) {
                if (pNode->nVersion < MIN_DEX_VERSION) {
                    continue;
                }

                if (pNode->addr != pfrom->addr) {
                    pNode->PushMessage(NetMsgType::DEXOFFEDIT, offer, vchSign);
                }
            }

            ReleaseNodeVector(vNodesCopy);
        }
    } else {
        LogPrint("dex", "DEXOFFEDIT -- offer check fail\n");
    }
}

std::list<std::pair<uint256, int>> CDexManager::availableOfferHashAndVersion() const
{
    std::list<std::pair<uint256, int>> list;

    for (auto offer : db->getOffersSell()) {
        list.push_back(std::make_pair(offer.hash, offer.editingVersion));
    }

    for (auto offer : db->getOffersBuy()) {
        list.push_back(std::make_pair(offer.hash, offer.editingVersion));
    }

    for (auto offer : uncOffers->getOffers()) {
        list.push_back(std::make_pair(offer.hash, offer.editingVersion));
    }

    return list;
}

CDexOffer CDexManager::getOfferInfo(const uint256 &hash) const
{
    if (db->isExistOfferSellByHash(hash)) {
        return CDexOffer(db->getOfferSellByHash(hash), Sell);
    }

    if (db->isExistOfferBuyByHash(hash)) {
        return CDexOffer(db->getOfferBuyByHash(hash), Buy);
    }

    if (uncOffers->isExistOffer(hash)) {
        return uncOffers->getOffer(hash);
    }

    return CDexOffer();
}

UnconfirmedOffers *CDexManager::getUncOffers() const
{
    return uncOffers;
}

void CDexManager::saveMyOffer(const MyOfferInfo &info)
{
    if (db->isExistMyOfferByHash(info.hash)) {
        db->editMyOffer(info);
    } else {
        db->addMyOffer(info);
    }
}




void ThreadDexManager()
{
    int step = 0;
    int minPeriod = 60000;

    const int stepCheckUnc = 1;
    const int stepDeleteOldUnc = 30;
    const int stepDeleteOld = 60;

    while (true) {
        MilliSleep(minPeriod);

        if (masternodeSync.IsSynced() && dexsync.statusSync() == CDexSync::NoStarted) {
            CheckDexMasternode();
            dexman.startSyncDex();
        }

        if (!dexsync.isSynced()) {
            continue;
        }

        CheckDexMasternode();

        if (step % stepCheckUnc == 0) {
            LogPrint("dex", "ThreadDexManager -- check unconfirmed offers\n");
            dexman.checkUncOffers();
        }

        if (step % stepDeleteOldUnc == 0) {
            LogPrint("dex", "ThreadDexManager -- delete old unconfirmed offers\n");
            dexman.deleteOldUncOffers();
        }

        if (step % stepDeleteOld == 0) {
            LogPrint("dex", "ThreadDexManager -- delete old offers\n");
            dexman.deleteOldOffers();
            LogPrint("dex", "ThreadDexManager -- set status expired for MyOffers\n");
            dexman.setStatusExpiredForMyOffers();
        }

        if (step == 60) {
            step = 0;
        } else {
            step++;
        }
    }
}

void CheckDexMasternode()
{
    int nOutbound = 0;
    int nDex = 0;
    std::vector<CNode *> nodeToRemove;
    std::set<std::vector<unsigned char> > setConnected;

    auto vNodesCopy = CopyNodeVector();

    for (auto pNode : vNodesCopy) {
        if (!pNode->fInbound && !pNode->fMasternode) {
            nOutbound++;

            if (pNode->nVersion >= MIN_DEX_VERSION && mnodeman.isExist(pNode)) {
                nDex++;
            } else {
                nodeToRemove.push_back(pNode);
            }
        }
    }

    int minNumDexNode = dexsync.minNumDexNode();
    if ((MAX_OUTBOUND_CONNECTIONS == nOutbound) && nDex < minNumDexNode) {
        int nDis = minNumDexNode - nDex;

        for (int i = 0; i < nDis; i++) {
            if (nodeToRemove.size() > i) {
                nodeToRemove[i]->fDisconnect = true;
            } else {
                break;
            }
        }
    }

    ReleaseNodeVector(vNodesCopy);
}

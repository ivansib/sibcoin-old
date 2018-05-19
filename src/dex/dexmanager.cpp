
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


namespace dex {

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

void CDexManager::addOrEditDraftMyOffer(MyOfferInfo &myOffer, bool usethread)
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

    saveMyOffer(myOffer, usethread);
}

void CDexManager::prepareAndSendMyOffer(MyOfferInfo &myOffer, std::string &error, bool usethread)
{
    initDB();

    CDexOffer dexOffer;

    if (myOffer.status == Indefined || myOffer.status == Draft) {
        uint256 oldHash = myOffer.hash;
        dexOffer.Create(myOffer);

        if (oldHash != dexOffer.hash) {
            db->deleteMyOfferByHash(oldHash, usethread);
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
            db->editOfferBuy(dex.offer, usethread);
        }
        if (dex.offer.isSell()) {
            db->editOfferSell(dex.offer, usethread);
        }
    } else if (dex.PayForOffer(tx, error)) {
        dexman.sendNewOffer(dex.offer);

        myOffer.setOfferInfo(dex.offer);
        myOffer.status = Active;
        if (dex.offer.isBuy()) {
            db->addOfferBuy(dex.offer, usethread);
        }
        if (dex.offer.isSell()) {
            db->addOfferSell(dex.offer, usethread);
        }
    }

    if (error.empty()) {
        saveMyOffer(myOffer, usethread);
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
    auto list = uncOffers->getAllOffers();
    std::vector<std::list<CDexOffer>::const_iterator> offersBuy;
    std::vector<std::list<CDexOffer>::const_iterator> offersSell;
    std::vector<CDexOffer> offersRemove;

    for (auto it = list.cbegin(); it != list.cend(); it++) {
        CDex dex((*it));
        std::string error;
        if (dex.CheckOfferTx(error)) {
            if ((*it).isBuy())  {
                if (!db->isExistOfferBuyByHash((*it).hash)) {
                  offersBuy.push_back(it);
                }
            }

            if ((*it).isSell())  {
                if (!db->isExistOfferSellByHash((*it).hash)) {
                  offersSell.push_back(it);
                }
            }

            offersRemove.push_back(*it);;
        }
    }

    sqlite3pp::transaction tx(*(db->getDB()));
    for (auto offer : offersBuy) {
        db->addOfferBuy(*offer, false);
    }
    for (auto offer : offersSell) {
        db->addOfferSell(*offer, false);
    }
    tx.commit();
    uncOffers->removeOffers(offersRemove);
}

void CDexManager::setStatusExpiredForMyOffers()
{
    if (db->setStatusExpiredForMyOffers() != 0) {
        LogPrint("dex", "setStatusExpiredForMyOffers() DB error: %s\n", db->getErrMsg().c_str());
    }
}

void CDexManager::deleteOldUncOffers()
{
    uncOffers->deleteOldOffers();
}

void CDexManager::deleteOldOffers()
{
    db->deleteOldOffersBuy(false);
    db->deleteOldOffersSell(false);
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
                if (db->isExistOfferBuyByHash(offer.hash)) {
                  bFound = true;
                } else {
                    db->addOfferBuy(offer, false);
                }
            }

            if (offer.isSell())  {
                if (db->isExistOfferSellByHash(offer.hash)) {
                  bFound = true;
                } else {
                    db->addOfferSell(offer, false);
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
            if (uncOffers->putOffer(offer)) {

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
        Misbehaving(pfrom->GetId(), 20);
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
                if (db->isExistOfferBuyByHash(offer.hash)) {
                    db->deleteOfferBuyByHash(offer.hash, false);
                    bFound = true;
                }
            }

            if (offer.isSell())  {
                if (db->isExistOfferSellByHash(offer.hash)) {
                    db->deleteOfferSellByHash(offer.hash, false);
                    bFound = true;
                }
            }
            if (!bFound) {
                bFound = uncOffers->removeOffer(offer);

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
        Misbehaving(pfrom->GetId(), 20);
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
    if (offer.Check(true)) {
        if (offer.CheckEditSign()) {
            CDex dex(offer);
            std::string error;
            bool isActual = false;
            if (dex.CheckOfferTx(error)) {
                if (offer.isBuy()) {
                    if (db->isExistOfferBuyByHash(offer.hash)) {
                        OfferInfo existOffer = db->getOfferBuyByHash(offer.hash);
                        if (offer.editingVersion > existOffer.editingVersion) {
                            db->editOfferBuy(offer, false);
                            isActual = true;
                        }
                    } else {
                        db->addOfferBuy(offer, false);
                        isActual = true;
                    }
                }

                if (offer.isSell()) {
                    if (db->isExistOfferSellByHash(offer.hash)) {
                        OfferInfo existOffer = db->getOfferSellByHash(offer.hash);
                        if (offer.editingVersion > existOffer.editingVersion) {
                            db->editOfferSell(offer, false);
                            isActual = true;
                        }
                    } else {
                        db->addOfferSell(offer, false);
                        isActual = true;
                    }
                }
                LogPrint("dex", "DEXOFFEDIT --\n%s\nactual %d\n", offer.dump().c_str(), isActual);
            } else {
                isActual = uncOffers->updateOffer(offer);
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
            LogPrint("dex", "DEXOFFEDIT -- reach the limit for editing a offer\n");
            Misbehaving(pfrom->GetId(), 5);
        }
    } else {
        LogPrint("dex", "DEXOFFEDIT -- offer check fail\n");
        Misbehaving(pfrom->GetId(), 20);
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

    for (auto offer : uncOffers->getAllOffers()) {
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

    CDexOffer uncOffer = uncOffers->getOfferByHash(hash);
    if (!uncOffer.IsNull()) {
        return uncOffer;
    }

    return CDexOffer();
}

UnconfirmedOffers *CDexManager::getUncOffers() const
{
    return uncOffers;
}

void CDexManager::saveMyOffer(const MyOfferInfo &info, bool usethread)
{
    if (db->isExistMyOfferByHash(info.hash)) {
        db->editMyOffer(info, usethread);
    } else {
        db->addMyOffer(info, usethread);
    }
}

}

using namespace dex;

void ThreadDexManager()
{
    int step = 0;
    int minPeriod = 60000;

    const int stepDeleteOld = 60;

    while (true) {
        MilliSleep(minPeriod);

        if (masternodeSync.IsSynced() && dexsync.statusSync() == CDexSync::NoStarted) {
            CheckDexMasternode();
            dexman.startSyncDex();
        }

        if (!dexsync.isSynced() && step != 0) {
            continue;
        }

        CheckDexMasternode();

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

void ThreadDexUncManager()
{
    int step = 0;
    int minPeriod = 60000;

    const int stepCheckUnc = 1;
    const int stepDeleteOldUnc = 30;

    while (true) {
        MilliSleep(minPeriod);

        if (!dexsync.isSynced()) {
            continue;
        }

        if (step % stepCheckUnc == 0) {
            LogPrint("dex", "ThreadDexManager -- check unconfirmed offers\n");
            dexman.checkUncOffers();
        }

        if (step % stepDeleteOldUnc == 0) {
            LogPrint("dex", "ThreadDexManager -- delete old unconfirmed offers\n");
            dexman.deleteOldUncOffers();
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

            LogPrint("dex", "CheckDexMasternode -- outbound node: %s\n", pNode->addr.ToString());

            if (pNode->nVersion >= MIN_DEX_VERSION && mnodeman.isExist(pNode)) {
                LogPrint("dex", "CheckDexMasternode -- dex node: %s\n", pNode->addr.ToString());
                nDex++;
            } else {
                LogPrint("dex", "CheckDexMasternode -- node to remove: %s\n", pNode->addr.ToString());
                nodeToRemove.push_back(pNode);
            }
        }
    }

    int minNumDexNode = dexsync.minNumDexNode();
    if ((MAX_OUTBOUND_CONNECTIONS == nOutbound) && nDex < minNumDexNode) {
        unsigned nDis = minNumDexNode - nDex;

        for (unsigned i = 0; i < nDis; i++) {
            if (nodeToRemove.size() > i) {
                nodeToRemove[i]->fDisconnect = true;
            } else {
                break;
            }
        }
    }

    ReleaseNodeVector(vNodesCopy);
}

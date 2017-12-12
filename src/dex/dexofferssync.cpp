#include <algorithm>
#include "dexofferssync.h"
#include "dexoffer.h"
#include "utiltime.h"
#include "net.h"
#include "util.h"

DexOffersSync dexOffersSync;

DexOffersSync::DexOffersSync()
{
    db = nullptr;
}

DexOffersSync::~DexOffersSync()
{
    delete db;
}

void DexOffersSync::ProcessMessage(CNode *pfrom, std::string &strCommand, CDataStream &vRecv)
{
    if (db == nullptr) {
        db = new DexDB(strDexDbFile);
    }

    if (strCommand == NetMsgType::DEXSYNCGETALLHASH) {
        auto hashs = availableOfferHash();

        if (hashs.size() > 0) {
            LogPrintf("DEXSYNCGETALLHASH -- send list hashes\n");
            pfrom->PushMessage(NetMsgType::DEXSYNCALLHASH, hashs);
        }
    } else if (strCommand == NetMsgType::DEXSYNCALLHASH) {
        LogPrintf("DEXSYNCALLHASH -- get list hashes\n");

        std::list<uint256> nodeHashs;
        vRecv >> nodeHashs;
        auto hashs = availableOfferHash();

        for (auto h : nodeHashs) {
            bool found = (std::find(hashs.begin(), hashs.end(), h) != hashs.end());

            if (!found) {
                LogPrintf("DEXSYNCALLHASH -- send a request for get offer info with hash = %s\n", h.GetHex().c_str());
                pfrom->PushMessage(NetMsgType::DEXSYNCGETOFFER, h);
            }
        }
    } else if (strCommand == NetMsgType::DEXSYNCGETOFFER) {
        uint256 hash;
        vRecv >> hash;

        auto offerInfo = getOfferInfo(hash);
        CDexOffer offer(offerInfo.first, offerInfo.second);
        LogPrintf("DEXSYNCGETOFFER -- send offer info with hash = %s\n", hash.GetHex().c_str());
        pfrom->PushMessage(NetMsgType::DEXSYNCOFFER, offer);
    } else if (strCommand == NetMsgType::DEXSYNCOFFER) {
        CDexOffer offer;
        vRecv >> offer;

        LogPrintf("DEXSYNCOFFER -- get offer info with hash = %s\n", offer.hash.GetHex().c_str());

        if (offer.isBuy())  {
            if (!db->isExistOfferBuy(offer.idTransaction)) {
                db->addOfferBuy(offer);
            }
        } else if (offer.isSell())  {
            if (!db->isExistOfferSell(offer.idTransaction)) {
                db->addOfferSell(offer);
            }
        }
    }
}

std::list<uint256> DexOffersSync::availableOfferHash()
{
    auto list = db->getSellHashs();
    auto listBuy = db->getBuyHashs();

    for (auto item : listBuy) {
        list.push_back(item);
    }

    return list;
}

std::pair<OfferInfo, TypeOffer> DexOffersSync::getOfferInfo(const uint256 &hash)
{
    OfferInfo offer;
    TypeOffer type;

    if (db->isExistOfferSellByHash(hash)) {
        offer = db->getOfferSellByHash(hash);
        type = Sell;
    }

    if (db->isExistOfferBuyByHash(hash)) {
        offer = db->getOfferBuyByHash(hash);
        type = Buy;
    }

    return std::make_pair(offer, type);
}

void ThreadOffersSync()
{
//    while (true)
//    {
//        MilliSleep(5000);

        std::vector<CNode*> vNodesCopy = CopyNodeVector();

        for (auto node : vNodesCopy) {
            if(node->fMasternode || (fMasterNode && node->fInbound)) {
                continue;
            }

            node->PushMessage(NetMsgType::DEXSYNCGETALLHASH);
        }
//    }
}

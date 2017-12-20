
#ifndef __DEX_MANAGER_H__
#define __DEX_MANAGER_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "dex/dexdto.h"
#include "dex/dexdb.h"
#include "dex.h"
#include "dexoffer.h"
#include "unconfirmedoffers.h"

class CDexManager;
extern CDexManager dexman;

using namespace dex;

class CDexManager
{
public:
    CDexManager();
    ~CDexManager();

    void ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);

    void sendOffer(const CDexOffer &offer);
    void deleteOldUncOffers();

private:
    DexDB *db;
    UnconfirmedOffers *uncOffers;

    std::list<uint256> availableOfferHash();
    CDexOffer getOfferInfo(const uint256 &hash);
};

void ThreadDexManager();

#endif // __DEX_MANAGER_H__

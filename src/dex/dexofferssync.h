#ifndef DEXOFFERSSYNC_H
#define DEXOFFERSSYNC_H

#include "net.h"
#include "dex/dexdb.h"

using namespace dex;

class DexOffersSync;
extern DexOffersSync dexOffersSync;

class DexOffersSync
{
public:
    DexOffersSync();
    ~DexOffersSync();

    void ProcessMessage(CNode *pfrom, std::string &strCommand, CDataStream &vRecv);

private:
    DexDB *db;

    std::list<uint256> availableOfferHash();
    std::pair<OfferInfo, TypeOffer> getOfferInfo(const uint256 &hash);
};

void ThreadOffersSync();

#endif

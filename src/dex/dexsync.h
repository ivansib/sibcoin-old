#ifndef DEXSYNC_H
#define DEXSYNC_H

#include "net.h"
#include "dex/db/dexdb.h"

class CDexSync;
extern CDexSync dexsync;

using namespace dex;

class CDexSync
{
public:
    CDexSync();
    ~CDexSync();

    void ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);

    void startSyncDex();
    void connectSignals();
    bool isSynced();

private:
    DexDB *db;

    void initDB();
    void sendHashOffers(CNode* pfrom) const;
    void getHashsAndSendRequestForGetOffers(CNode* pfrom, CDataStream& vRecv) const;
    void sendOffer(CNode* pfrom, CDataStream& vRecv) const;
    void getOfferAndSaveInDb(CDataStream& vRecv);

    bool isSync;
};

void DexConnectSignals();

#endif //DEXSYNC_H

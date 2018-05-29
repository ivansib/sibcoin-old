#ifndef DEXSYNC_H
#define DEXSYNC_H

#include "timer.h"
#include "dexsyncinfo.h"
#include "net.h"
#include "dex/db/dexdb.h"

namespace dex {

class CDexSync;
extern CDexSync dexsync;

class CDexSync
{
public:
    enum class Status {
        NoStarted,
        Started,
        Initial,
        SyncStepOne,
        SyncStepSecond,
        Finished
    };

    enum class StatusOffers {
        Empty,
        Actual
    };

    enum class StatusNode {
        Bad,
        Good,
        Process,
        Actual
    };

    CDexSync();
    ~CDexSync();

    void ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);

    void startSyncDex();
    void finishSyncDex();
    void connectSignals();
    bool isSynced() const;
    std::string getSyncStatus() const;
    Status statusSync();
    int minNumDexNode() const;
    bool reset();
    void updatePrevData();
    bool checkSyncData();
    void startTimer();
    void setRunTimer(const bool b);
    void startTimerForAnswer() const;
    int offersNeedDownloadSize() const;
    void sendRequestForGetOffers();
    void checkNodes();
    bool actualSync() const;

    boost::signals2::signal<void()> syncFinished;

    std::set<uint256> getOffersNeedDownload() const;

private:
    DexDB *db;

    void initDB();
    void sendHashOffers(CNode* pfrom, CDataStream& vRecv) const;
    void getHashs(CNode* pfrom, CDataStream& vRecv);
    void sendOffer(CNode* pfrom, CDataStream& vRecv) const;
    void getOfferAndSaveInDb(CNode* pfrom, CDataStream& vRecv);
    void noOffersList(CNode* pfrom, CDataStream& vRecv);
    void noHash(CNode* pfrom, CDataStream& vRecv);

    void eraseItemFromOffersNeedDownload(const uint256 &hash);
    bool canStart();
    void initSetWaitAnswerFromNodes(const std::vector<CNode*> &nodes);
    void addAddrToStatusNode(const CAddress &addr, StatusNode status);
    DexSyncInfo dexSyncInfo() const;

    std::set<CAddress> waitAnswerFromNodes;
    std::map<CAddress, StatusNode> statusNodes;
    std::set<uint256> offersNeedDownload;
    size_t maxOffersNeedDownload;
    size_t prevOffersNeedDownloadSize;
    size_t prevMaxOffersNeedDownload;
    float statusPercent;

    Status status;
    bool isRunTimer;
};

void DexConnectSignals();
void FinishSyncDex(); // NODE: rename
void StopTimerForAnswer();

}

#endif //DEXSYNC_H

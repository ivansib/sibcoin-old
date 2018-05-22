#ifndef DEXSYNC_H
#define DEXSYNC_H

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "net.h"
#include "dex/db/dexdb.h"

namespace dex {

class CDexSync;
extern CDexSync dexsync;

class  Timer {
private:
    boost::shared_ptr<boost::thread> thr;

    template <typename Callable>
    class func {
        unsigned int dur;
        Callable& cb;

    public:
        func (long long dur, const Callable& cb): dur (dur), cb((Callable&)cb) {}
        void operator() () {
            boost::this_thread::sleep(boost::posix_time::milliseconds(dur));
            cb();
        }
    };

public:
    template <typename Callable> Timer (long long dur, const  Callable& cb)  {
        func<Callable> ff (dur, cb);
        thr.reset(new boost::thread(ff));
    }

    ~Timer() {}
};

class CDexSync
{
public:
    enum Status {
        NoStarted,
        Started,
        Initial,
        SyncStepOne,
        SyncStepSecond,
        Finished
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
    void startTimer() const;
    void startTimerForAnswer() const;
    int offersNeedDownloadSize() const;
    void sendRequestForGetOffers() const;
    void addToListBadNodes();

    boost::signals2::signal<void()> syncFinished;

    std::set<uint256> getOffersNeedDownload() const;
    void setOffersNeedDownload(const std::set<uint256> &value);

private:
    DexDB *db;

    void initDB();
    void sendHashOffers(CNode* pfrom) const;
    void getHashs(CNode* pfrom, CDataStream& vRecv);
    void sendOffer(CNode* pfrom, CDataStream& vRecv) const;
    void getOfferAndSaveInDb(CNode* pfrom, CDataStream& vRecv);
    void noOffersList(CNode* pfrom);
    void noHash(CNode* pfrom, CDataStream& vRecv);

    void eraseItemFromOffersNeedDownload(const uint256 &hash);
    bool canStart();
    void initSetWaitAnswerFromNodes(const std::vector<CNode*> &nodes);
    void addAddrToGoodNode(const CAddress &addr, bool type);

    std::set<CAddress> waitAnswerFromNodes;
    std::map<CAddress, bool> goodNodes;
    std::set<uint256> offersNeedDownload;
    size_t maxOffersNeedDownload;
    size_t prevOffersNeedDownloadSize;
    size_t prevMaxOffersNeedDownload;
    float statusPercent;

    Status status;
};

void DexConnectSignals();
void FinishSyncDex(); // NODE: rename
void StopTimerForAnswer();

}

#endif //DEXSYNC_H

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
        NoRestarted,
        Started,
        Restarted,
        Initial,
        Sync,
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
    void restart();
    void updatePrevData();
    bool checkSyncData();
    void startTimer();
    int offersNeedDownloadSize() const;

    boost::signals2::signal<void()> syncFinished;

private:
    DexDB *db;

    void initDB();
    void sendHashOffers(CNode* pfrom) const;
    void getHashsAndSendRequestForGetOffers(CNode* pfrom, CDataStream& vRecv);
    void sendOffer(CNode* pfrom, CDataStream& vRecv) const;
    void getOfferAndSaveInDb(CNode* pfrom, CDataStream& vRecv);
    void insertItemFromOffersNeedDownload(const uint256 &hash);
    void eraseItemFromOffersNeedDownload(const uint256 &hash);
    bool canStart();

    std::set<uint256> offersNeedDownload;
    size_t maxOffersNeedDownload;
    size_t prevOffersNeedDownloadSize;
    size_t prevMaxOffersNeedDownload;
    float statusPercent;

    Status status;
};

void DexConnectSignals();
void FinishSyncDex();

}

#endif //DEXSYNC_H

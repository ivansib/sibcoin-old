#include "workerthreaddexdb.h"
#include "dex/dexmanager.h"
#include "convertdata.h"

WorkerThreadDexDB::WorkerThreadDexDB(QObject *parent) : QObject(parent)
{

}

WorkerThreadDexDB::~WorkerThreadDexDB()
{

}

void WorkerThreadDexDB::prepareAndSendMyOffer(const QtMyOfferInfo &info)
{
    MyOfferInfo myOffer = ConvertData::fromQtMyOfferInfo(info);
    std::string error;
    dexman.prepareAndSendMyOffer(myOffer, error);

    if (!error.empty()) {
        Q_EMIT errorPrepareAndSendMyOffer(tr(error.c_str()));
    }
}

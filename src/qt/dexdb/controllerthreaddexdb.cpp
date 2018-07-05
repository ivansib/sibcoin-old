#include "controllerthreaddexdb.h"
#include "workerthreaddexdb.h"

ControllerThreadDexDB::ControllerThreadDexDB(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QtMyOfferInfo>("QtMyOfferInfo");

    WorkerThreadDexDB *worker = new WorkerThreadDexDB();
    worker->moveToThread(&thread);

    connect(&thread, SIGNAL(finished()), worker, SLOT(deleteLater()));

    connect(this, SIGNAL(myOfferPrepareAndSend(QtMyOfferInfo)), worker, SLOT(prepareAndSendMyOffer(QtMyOfferInfo)));
    connect(worker, SIGNAL(errorPrepareAndSendMyOffer(QString)), this, SIGNAL(errorPrepareAndSendMyOffer(QString)));

    thread.start();
}

ControllerThreadDexDB::~ControllerThreadDexDB()
{
    thread.quit();
    thread.wait();
}

void ControllerThreadDexDB::prepareAndSendMyOffer(const QtMyOfferInfo &info)
{
    Q_EMIT myOfferPrepareAndSend(info);
}

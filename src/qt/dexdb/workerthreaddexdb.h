#ifndef WORKERTHREADDEXDB_H
#define WORKERTHREADDEXDB_H

#include <QObject>
#include "dto.h"

class WorkerThreadDexDB : public QObject
{
    Q_OBJECT

public:
    WorkerThreadDexDB(QObject *parent = nullptr);
    ~WorkerThreadDexDB();

public Q_SLOTS:
    void prepareAndSendMyOffer(const QtMyOfferInfo &info);

Q_SIGNALS:
    void errorPrepareAndSendMyOffer(const QString &error);
};

#endif //WORKERTHREADDEXDB_H

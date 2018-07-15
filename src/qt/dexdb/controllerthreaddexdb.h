#ifndef CONTROLLERTHREADDEXDB_H
#define CONTROLLERTHREADDEXDB_H

#include <QObject>
#include <QThread>
#include "dto.h"

Q_DECLARE_METATYPE(QtMyOfferInfo)

class ControllerThreadDexDB : public QObject
{
    Q_OBJECT

public:
    ControllerThreadDexDB(QObject *parent = nullptr);
    ~ControllerThreadDexDB();

    void prepareAndSendMyOffer(const QtMyOfferInfo &info);

private:
    QThread thread;

Q_SIGNALS:
    void myOfferPrepareAndSend(const QtMyOfferInfo &info);
    void errorPrepareAndSendMyOffer(const QString &error);
};

#endif //CONTROLLERTHREADDEXDB_H

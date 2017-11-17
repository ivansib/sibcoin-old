#ifndef CALLBACKDBFORGUI_H
#define CALLBACKDBFORGUI_H

#include <QObject>
#include "dex/callbackdb.h"

using namespace dex;

Q_DECLARE_METATYPE(TypeTable)
Q_DECLARE_METATYPE(TypeTableOperation)
Q_DECLARE_METATYPE(StatusTableOperation)

class CallBackDbForGui : public QObject, public CallBackDB
{
    Q_OBJECT

public:
    CallBackDbForGui(QObject *parent = nullptr);
    ~CallBackDbForGui() {}

    virtual void finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);

Q_SIGNALS:
    void tableOperationFinished(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);
};

#endif

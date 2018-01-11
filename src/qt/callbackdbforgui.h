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

    CallBackDbForGui();
    CallBackDbForGui(const CallBackDbForGui &) {}
    CallBackDbForGui &operator=(const CallBackDbForGui &) {return *this;}

    static CallBackDbForGui *pSingleton;
    static int nCounter;

public:
    static CallBackDbForGui *instance() {
        if (pSingleton == nullptr) {
            pSingleton = new CallBackDbForGui();
        } else {
            nCounter++;
        }

        return pSingleton;
    }

    static void freeInstance() {
        if (nCounter > 0) {
            nCounter--;

            if (nCounter == 0) {
                delete pSingleton;
                pSingleton = nullptr;
            }
        }
    }

    virtual void finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);

Q_SIGNALS:
    void tableOperationFinished(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);
};

#endif

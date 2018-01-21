#ifndef CALLBACKDBFORRPC_H
#define CALLBACKDBFORRPC_H

#include "dex/callbackdb.h"
#include "dex/dexdto.h"

using namespace dex;

enum CallBackStatus
{
    NotDone,
    Ok,
    Error
};

class CallBackDBForRpc : public CallBackDB
{
public:
    CallBackDBForRpc();

    CallBackStatus statusAddMyOffer() const;
    CallBackStatus statusChangedMyOffer() const;

    virtual void finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status);

private:
    CallBackStatus addMyOffer;
    CallBackStatus changedMyOffer;
};


#endif

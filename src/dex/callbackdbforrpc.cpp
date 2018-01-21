#include "callbackdbforrpc.h"

CallBackDBForRpc::CallBackDBForRpc()
{
    addMyOffer = NotDone;
    changedMyOffer = NotDone;
}

CallBackStatus CallBackDBForRpc::statusAddMyOffer() const
{
    return addMyOffer;
}

CallBackStatus CallBackDBForRpc::statusChangedMyOffer() const
{
    return changedMyOffer;
}

void CallBackDBForRpc::finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
{
    if (table == MyOffers && operation == Add) {
        if (status == StatusTableOperation::Ok) {
            addMyOffer = CallBackStatus::Ok;
        } else {
            addMyOffer = CallBackStatus::Error;
        }
    }

    if (table == MyOffers && (operation == Add || operation == Edit)) {
        if (status == StatusTableOperation::Ok) {
            changedMyOffer = CallBackStatus::Ok;
        } else {
            changedMyOffer = CallBackStatus::Error;
        }
    }
}

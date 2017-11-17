#ifndef CALLBACKDB_H
#define CALLBACKDB_H

#include "dexdto.h"

namespace dex {

class CallBackDB
{
public:
    CallBackDB() {}
    virtual ~CallBackDB() {}

    virtual void finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status) = 0;
};

}

#endif

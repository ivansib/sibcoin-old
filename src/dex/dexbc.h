
#ifndef __DEX_BROARCAST_H__
#define __DEX_BROARCAST_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "timedata.h"
#include "dex/dexdto.h"

#include "dex.h"


class CDexBroadcast : public CDex
{
public:
  

public:

    CDexBroadcast();
    CDexBroadcast(const CDex& dex);




/*
    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
    }
*/
};

#endif  // __DEX_BROARCAST_H__


#ifndef __DEX_H__
#define __DEX_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "timedata.h"

class CDex;
class CDexBroadcast;



//////////////////////////////////
class CDex
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

public:


    CDex();
    CDex(const CDex& other);
    CDex(const CDexBroadcast& mnb);
/*
    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        LOCK(cs);
    }
*/
//    void Check(bool fForce = false);


};



//////////////////////////////////////////////////////
class CDexBroadcast : public CDex
{
public:

    bool fRecovery;

    CDexBroadcast() : CDex(), fRecovery(false) {}
    CDexBroadcast(const CDex& dex) : CDex(dex), fRecovery(false) {}
/*
    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
    }
*/
};


#endif

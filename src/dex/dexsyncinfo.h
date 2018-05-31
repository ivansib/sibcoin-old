#ifndef DEXSYNCINFO_H
#define DEXSYNCINFO_H

#include "net.h"
#include "dexcrc.h"

namespace dex {

struct DexSyncInfo
{
    CDexCrc checkSum;
    int count;
    uint64_t lastTimeMod;

    DexSyncInfo() {
        checkSum = CDexCrc();
        count = 0;
        lastTimeMod = 0;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(checkSum);
        READWRITE(count);
        READWRITE(lastTimeMod);
    }

    bool isNull() {
//        if (checkSum != 0) {
//            return false;
//        }

        if (count != 0) {
            return false;
        }

        if (lastTimeMod != 0) {
            return false;
        }

        return true;
    }

    friend bool operator==(const DexSyncInfo &a, const DexSyncInfo &b) {
        if (a.checkSum != b.checkSum) {
            return false;
        }

        if (a.count != b.count) {
            return false;
        }

        if (a.lastTimeMod != b.lastTimeMod) {
            return false;
        }

        return true;
    }

    friend bool operator!=(const DexSyncInfo &a, const DexSyncInfo &b) {
        return !(a == b);
    }
};

}

#endif // DEXSYNCINFO_H

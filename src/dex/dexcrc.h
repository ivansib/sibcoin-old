#ifndef __DEX_CRC_H__
#define __DEX_CRC_H__

#include "uint256.h"
#include "main.h"
#include "dexoffer.h"


namespace dex {

class CDexCrc {
    mutable CCriticalSection cs;

public:
    uint256  hashsum;
    uint256  hashxor;
    uint32_t editingVersionSum;

public:
    CDexCrc();
    CDexCrc(const CDexOffer &off);
    CDexCrc(const OfferInfo &info);
    CDexCrc(const MyOfferInfo &info);
    CDexCrc(const CDexCrc &crc);
    CDexCrc(const uint256 &hsum, const uint256 &hxor, const uint32_t &evsum);

    CDexCrc& operator=(const CDexCrc&);
    CDexCrc& operator=(const CDexOffer &off);
    CDexCrc& operator=(const OfferInfo &info);
    CDexCrc& operator=(const MyOfferInfo &info);

    bool operator==(const CDexCrc&) const;
    bool operator!=(const CDexCrc&) const;

    CDexCrc operator+(const CDexCrc&) const;
    CDexCrc& operator+=(const CDexCrc&);
    CDexCrc& operator+=(const CDexOffer &off);
    CDexCrc& operator+=(const OfferInfo &info);
    CDexCrc& operator+=(const MyOfferInfo &info);

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        LOCK(cs);
        READWRITE(hashsum);
        READWRITE(hashxor);
        READWRITE(editingVersionSum);
    }

private:
    void add256(const uint256 &a, const uint256 &b, uint256 &r) const;
    void xor256(const uint256 &a, const uint256 &b, uint256 &r) const;
};


}  // namespace dex

#endif //__DEX_CRC_H__

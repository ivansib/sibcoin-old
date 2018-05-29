#include "dexcrc.h"


namespace dex {


CDexCrc::CDexCrc()
{
    editingVersionSum = 0;
}


CDexCrc::CDexCrc(const CDexOffer &off)
{
    hashsum = off.hash;
    hashxor = off.hash;
    editingVersionSum = off.editingVersion;
}


CDexCrc::CDexCrc(const OfferInfo &info)
{
    hashsum = info.hash;
    hashxor = info.hash;
    editingVersionSum = info.editingVersion;
}


CDexCrc::CDexCrc(const MyOfferInfo &info)
{
    hashsum = info.hash;
    hashxor = info.hash;
    editingVersionSum = info.editingVersion;
}


CDexCrc::CDexCrc(const CDexCrc &crc)
{
    hashsum = crc.hashsum;
    hashxor = crc.hashxor;
    editingVersionSum = crc.editingVersionSum;
}


CDexCrc::CDexCrc(const uint256 &hsum, const uint256 &hxor, const uint32_t &evsum)
{
    hashsum = hsum;
    hashxor = hxor;
    editingVersionSum = evsum;
}


CDexCrc::CDexCrc(const std::list<std::pair<uint256, uint32_t> > &hashlist)
{
    editingVersionSum = 0;
    *this += hashlist;
}


CDexCrc::CDexCrc(const std::list<OfferInfo> &offlist)
{
    editingVersionSum = 0;
    *this += offlist;
}

CDexCrc::CDexCrc(const std::list<MyOfferInfo> &offlist)
{
    editingVersionSum = 0;
    *this += offlist;
}

CDexCrc::CDexCrc(const std::list<CDexOffer> &offlist)
{
    editingVersionSum = 0;
    *this += offlist;
}


CDexCrc& CDexCrc::operator=(const CDexCrc& crc)
{
    hashsum = crc.hashsum;
    hashxor = crc.hashxor;
    editingVersionSum = crc.editingVersionSum;
    return *this;
}

CDexCrc& CDexCrc::operator=(const CDexOffer &off)
{
    hashsum = off.hash;
    hashxor = off.hash;
    editingVersionSum = off.editingVersion;
    return *this;
}


CDexCrc& CDexCrc::operator=(const OfferInfo &info)
{
    hashsum = info.hash;
    hashxor = info.hash;
    editingVersionSum = info.editingVersion;
    return *this;
}


CDexCrc& CDexCrc::operator=(const MyOfferInfo &info)
{
    hashsum = info.hash;
    hashxor = info.hash;
    editingVersionSum = info.editingVersion;
    return *this;
}


CDexCrc CDexCrc::operator+(const CDexCrc& crc) const
{
    CDexCrc result(*this);
    result += crc;
    return result;
}

CDexCrc CDexCrc::operator+(const CDexOffer &off) const
{
    CDexCrc result(*this);
    result += off;
    return result;
}


CDexCrc CDexCrc::operator+(const OfferInfo &info) const
{
    CDexCrc result(*this);
    result += info;
    return result;
}


CDexCrc CDexCrc::operator+(const MyOfferInfo &info) const
{
    CDexCrc result(*this);
    result += info;
    return result;
}


CDexCrc CDexCrc::operator+(const std::list<std::pair<uint256, uint32_t> > &hashlist) const
{
    CDexCrc result(*this);
    result += hashlist;
    return result;
}


CDexCrc CDexCrc::operator+(const std::list<OfferInfo> &offlist) const
{
    CDexCrc result(*this);
    result += offlist;
    return result;
}


CDexCrc CDexCrc::operator+(const std::list<MyOfferInfo> &offlist) const
{
    CDexCrc result(*this);
    result += offlist;
    return result;
}


CDexCrc CDexCrc::operator+(const std::list<CDexOffer> &offlist) const
{
    CDexCrc result(*this);
    result += offlist;
    return result;
}


CDexCrc& CDexCrc::operator+=(const CDexCrc& crc)
{
    add256(hashsum, crc.hashsum, hashsum);
    xor256(hashxor, crc.hashxor, hashxor);
    editingVersionSum += crc.editingVersionSum;
    return *this;
}


CDexCrc& CDexCrc::operator+=(const CDexOffer &off)
{
    add256(hashsum, off.hash, hashsum);
    xor256(hashxor, off.hash, hashxor);
    editingVersionSum += off.editingVersion;
    return *this;
}

CDexCrc& CDexCrc::operator+=(const OfferInfo &info)
{
    add256(hashsum, info.hash, hashsum);
    xor256(hashxor, info.hash, hashxor);
    editingVersionSum += info.editingVersion;
    return *this;
}

CDexCrc& CDexCrc::operator+=(const MyOfferInfo &info)
{
    add256(hashsum, info.hash, hashsum);
    xor256(hashxor, info.hash, hashxor);
    editingVersionSum += info.editingVersion;
    return *this;
}

CDexCrc& CDexCrc::operator+=(const std::list<std::pair<uint256, uint32_t> > &hashlist)
{
    for (auto v : hashlist) {
        add256(hashsum, v.first, hashsum);
        xor256(hashxor, v.first, hashxor);
        editingVersionSum += v.second;
    }
    return *this;
}


CDexCrc& CDexCrc::operator+=(const std::list<OfferInfo> &offlist)
{
    for (auto v : offlist) {
        add256(hashsum, v.hash, hashsum);
        xor256(hashxor, v.hash, hashxor);
        editingVersionSum += v.editingVersion;
    }
    return *this;
}


CDexCrc& CDexCrc::operator+=(const std::list<MyOfferInfo> &offlist)
{
    for (auto v : offlist) {
        add256(hashsum, v.hash, hashsum);
        xor256(hashxor, v.hash, hashxor);
        editingVersionSum += v.editingVersion;
    }
    return *this;
}


CDexCrc& CDexCrc::operator+=(const std::list<CDexOffer> &offlist)
{
    for (auto v : offlist) {
        add256(hashsum, v.hash, hashsum);
        xor256(hashxor, v.hash, hashxor);
        editingVersionSum += v.editingVersion;
    }
    return *this;
}


bool CDexCrc::operator==(const CDexCrc& crc) const
{
  return (hashsum == crc.hashsum) && (hashxor == crc.hashxor) && (editingVersionSum == crc.editingVersionSum);
}

bool CDexCrc::operator!=(const CDexCrc& crc) const
{
  return (hashsum != crc.hashsum) || (hashxor != crc.hashxor) || (editingVersionSum != crc.editingVersionSum);
}



void CDexCrc::add256(const uint256 &a, const uint256 &b, uint256 &r) const
{
    uint8_t carry = 0;
    for (size_t i = 0; i < r.size(); i++) {
        uint16_t n = carry + static_cast<uint8_t>(a.begin()[i]) + static_cast<uint8_t>(b.begin()[i]);
        r.begin()[i] = static_cast<uint8_t>(n & 0xff);
        carry = n >> 8;
    }
}

void CDexCrc::xor256(const uint256 &a, const uint256 &b, uint256 &r) const
{
    for (size_t i = 0; i < r.size(); i++) {
        r.begin()[i] = static_cast<uint8_t>(a.begin()[i]) ^ static_cast<uint8_t>(b.begin()[i]);
    }
}





} // namespace dex

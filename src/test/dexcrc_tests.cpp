#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include "test/test_sibcoin.h"
#include "dex/db/dexdb.h"
#include "dex/dexoffer.h"
#include "dex/dexcrc.h"
#include "random.h"

using namespace dex;


BOOST_FIXTURE_TEST_SUITE(dexcrc, BasicTestingSetup)


void test_operation()
{
    OfferInfo offer;
    offer.hash.SetHex("0xff");
    offer.editingVersion = 5;

    CDexCrc crc;
    BOOST_TEST_MESSAGE("\n\thashsum\t" << crc.hashsum.ToString() << "\n\txor\t" << crc.hashxor.ToString() << "\n\tedver\t" << crc.editingVersionSum);

    crc = offer;
    BOOST_CHECK(crc.hashsum == offer.hash);

    crc += offer;
    BOOST_TEST_MESSAGE("\n\thashsum\t" << crc.hashsum.ToString() << "\n\txor\t" << crc.hashxor.ToString() << "\n\tedver\t" << crc.editingVersionSum);
    BOOST_CHECK(crc.hashsum != offer.hash);
    BOOST_CHECK(crc.hashxor != offer.hash);
    BOOST_CHECK(crc.editingVersionSum == 10);

    crc += offer;
    BOOST_CHECK(crc.hashsum.ToString() == "00000000000000000000000000000000000000000000000000000000000002fd"); // 0xFF + 0xFF + 0xFF = 0x2FD
    BOOST_CHECK(crc.hashxor.ToString() == "00000000000000000000000000000000000000000000000000000000000000ff"); // 0xFF ^ 0xFF ^ 0xFF = 0xFF
    BOOST_CHECK(crc.editingVersionSum == 15);

    BOOST_TEST_MESSAGE("\n\thashsum\t" << crc.hashsum.ToString() << "\n\txor\t" << crc.hashxor.ToString() << "\n\tedver\t" << crc.editingVersionSum);
}


void test_cumulative_add()
{
    OfferInfo offers[10];
    CDexCrc crc, crc1, crc2;

    for (int i = 0; i < 10; i++) {
        OfferInfo offer;
        offer.hash = GetRandHash();
        offer.editingVersion = static_cast<uint32_t>(GetRand(10));
        crc += offer;
        if (i%2) {
            crc1 += offer;
        } else {
            crc2 += offer;
        }
    }

    BOOST_CHECK(crc == crc1 + crc2);
}


BOOST_AUTO_TEST_CASE(dexcrc)
{
    test_operation();
    test_cumulative_add();
}

BOOST_AUTO_TEST_SUITE_END()

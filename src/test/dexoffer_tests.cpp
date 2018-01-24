#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include "test/test_sibcoin.h"
#include "dex/dexdb.h"
#include "dex/dexoffer.h"
#include "random.h"

using namespace dex;

void checkConvert()
{
    MyOfferInfo myOffer;
    myOffer.pubKey = GetRandHash().GetHex();
    myOffer.hash = GetRandHash();
    myOffer.idTransaction = GetRandHash();
    myOffer.type = Buy;
    myOffer.status = Active;
    myOffer.price = 1234567;
    myOffer.minAmount = 10000;
    myOffer.shortInfo = "first info";
    myOffer.countryIso = "RU";
    myOffer.currencyIso = "RUB";
    myOffer.paymentMethod = 1;
    myOffer.timeCreate = 1025668989;
    myOffer.timeToExpiration = 1027778989;
    myOffer.editingVersion = 5;

    OfferInfo offer = myOffer.getOfferInfo();

    CDexOffer dexOffer(myOffer);

    MyOfferInfo nMyOffer = MyOfferInfo(dexOffer);
    OfferInfo nOffer = OfferInfo(dexOffer);

    BOOST_CHECK(myOffer.pubKey == nMyOffer.pubKey);
    BOOST_CHECK(myOffer.idTransaction == nMyOffer.idTransaction);
    BOOST_CHECK(myOffer.hash == nMyOffer.hash);
    BOOST_CHECK(myOffer.type == nMyOffer.type);
    BOOST_CHECK(myOffer.status == nMyOffer.status);
    BOOST_CHECK(myOffer.price == nMyOffer.price);
    BOOST_CHECK(myOffer.minAmount == nMyOffer.minAmount);
    BOOST_CHECK(myOffer.shortInfo == nMyOffer.shortInfo);
    BOOST_CHECK(myOffer.countryIso == nMyOffer.countryIso);
    BOOST_CHECK(myOffer.currencyIso == nMyOffer.currencyIso);
    BOOST_CHECK(myOffer.paymentMethod == nMyOffer.paymentMethod);
    BOOST_CHECK(myOffer.timeCreate == nMyOffer.timeCreate);
    BOOST_CHECK(myOffer.timeToExpiration == nMyOffer.timeToExpiration);
    BOOST_CHECK(myOffer.editingVersion == nMyOffer.editingVersion);

    BOOST_CHECK(offer.pubKey == nOffer.pubKey);
    BOOST_CHECK(offer.idTransaction == nOffer.idTransaction);
    BOOST_CHECK(offer.hash == nOffer.hash);
    BOOST_CHECK(offer.price == nOffer.price);
    BOOST_CHECK(offer.minAmount == nOffer.minAmount);
    BOOST_CHECK(offer.shortInfo == nOffer.shortInfo);
    BOOST_CHECK(offer.countryIso == nOffer.countryIso);
    BOOST_CHECK(offer.currencyIso == nOffer.currencyIso);
    BOOST_CHECK(offer.paymentMethod == nOffer.paymentMethod);
    BOOST_CHECK(offer.timeCreate == nOffer.timeCreate);
    BOOST_CHECK(offer.timeToExpiration == nOffer.timeToExpiration);
    BOOST_CHECK(offer.editingVersion == nOffer.editingVersion);
}

BOOST_FIXTURE_TEST_SUITE(dexoffer_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(dexoffer_test1)
{
    checkConvert();
}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include "test/test_sibcoin.h"

#include "dex/dexdb.h"
#include "random.h"

using namespace dex;

void checkCountry(DexDB &db)
{
    auto cList = db.getCountriesInfo();
    int size = cList.size();
    BOOST_CHECK(size == 247);

    db.deleteCountry("RU");
    db.deleteCountry("US");

    cList = db.getCountriesInfo();
    size = cList.size();

    BOOST_CHECK(size == 245);

    auto front = cList.front();
    cList.pop_back();
    cList.push_back(front);

    db.editCountries(cList);
    cList = db.getCountriesInfo();
    auto back = cList.back();

    BOOST_CHECK(front.name == back.name && front.iso == back.iso && front.enabled == back.enabled);

    auto find = db.getCountryInfo(front.iso);

    BOOST_CHECK(find.name == front.name && find.iso == front.iso && find.enabled == front.enabled);
}

void checkCurrency(DexDB &db)
{
    auto cList = db.getCurrenciesInfo();
    int size = cList.size();
    BOOST_CHECK(size == 147);

    db.deleteCurrency("RUB");
    db.deleteCurrency("USD");

    cList = db.getCurrenciesInfo();
    size = cList.size();

    BOOST_CHECK(size == 145);

    auto front = cList.front();
    cList.pop_back();
    cList.push_back(front);

    db.editCurrencies(cList);
    cList = db.getCurrenciesInfo();
    auto back = cList.back();

    BOOST_CHECK(front.name == back.name && front.iso == back.iso && front.symbol == back.symbol && front.enabled == back.enabled);

    auto find = db.getCurrencyInfo(front.iso);

    BOOST_CHECK(find.name == front.name && find.iso == front.iso && find.symbol == front.symbol && find.enabled == front.enabled);
}

void checkPaymentMethod(DexDB &db)
{
    auto cList = db.getPaymentMethodsInfo();
    int size = cList.size();
    BOOST_CHECK(size == 2);

    auto front = cList.front();
    auto find = db.getPaymentMethodInfo(front.type);

    BOOST_CHECK(find.name == front.name && find.type == front.type && find.description == front.description);

    db.deletePaymentMethod(1);
    db.deletePaymentMethod(128);

    cList = db.getPaymentMethodsInfo();
    size = cList.size();

    BOOST_CHECK(size == 0);
}

void checkOffers(DexDB &db)
{
    OfferInfo info;
    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 1234567;
    info.minAmount = 10000;
    info.shortInfo = "first info";
    info.countryIso = "RU";
    info.currencyIso = "RUB";
    info.paymentMethod = 1;
    info.timeCreate = 946688461;
    info.timeToExpiration = 10;

    std::list<OfferInfo> iList;
    iList.push_back(info);
    db.addOfferBuy(info);
    db.addOfferSell(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 1555;
    info.minAmount = 0;
    info.shortInfo = "info";
    info.countryIso = "US";
    info.currencyIso = "USD";
    info.paymentMethod = 128;
    info.timeCreate = 996688461;
    info.timeToExpiration = 15;

    iList.push_back(info);
    db.addOfferBuy(info);
    db.addOfferSell(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 133;
    info.minAmount = 3;
    info.shortInfo = "info";
    info.countryIso = "UA";
    info.currencyIso = "UAN";
    info.paymentMethod = 128;
    info.timeCreate = 996998461;
    info.timeToExpiration = 9;

    iList.push_back(info);
    db.addOfferBuy(info);
    db.addOfferSell(info);

    BOOST_CHECK(db.isExistOfferBuy(iList.front().idTransaction));
    BOOST_CHECK(db.isExistOfferSell(iList.back().idTransaction));

    BOOST_CHECK(!db.isExistOfferBuy(GetRandHash()));
    BOOST_CHECK(!db.isExistOfferSell(GetRandHash()));
}

BOOST_FIXTURE_TEST_SUITE(dexdb_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(dexdb_test1)
{
    std::string dbName = "test.db";
    remove(dbName.c_str());
    DexDB db(dbName);

    checkCountry(db);
    checkCurrency(db);
    checkPaymentMethod(db);
    checkOffers(db);

//    remove(dbName.c_str());
}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include "test/test_sibcoin.h"

#include "dex/dexdb.h"
#include "dex/callbackdb.h"
#include "random.h"

using namespace dex;

class CallBackOffers : public CallBackDB
{
public:
    CallBackOffers() {
        init();
    }

    virtual void finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
    {
        if (operation == Add && status == Ok) {
            if (table == OffersSell) {
                addOffersSell++;
            }

            if (table == OffersBuy) {
                addOffersBuy++;
            }
        }

        if (operation == Delete && status == Ok) {
            if (table == OffersSell) {
                deleteOffersSell++;
            }

            if (table == OffersBuy) {
                deleteOffersBuy++;
            }
        }

        if (operation == Edit && status == Ok) {
            if (table == OffersSell) {
                editOffersSell++;
            }

            if (table == OffersBuy) {
                editOffersBuy++;
            }
        }
    }

    void init()
    {
        addOffersSell = 0;
        addOffersBuy = 0;
        deleteOffersSell = 0;
        deleteOffersBuy = 0;
        editOffersSell = 0;
        editOffersBuy = 0;
    }

    int getAddOffersSell()
    {
        return addOffersSell;
    }

    int getAddOffersBuy()
    {
        return addOffersBuy;
    }

    int getDeleteOffersSell()
    {
        return deleteOffersSell;
    }

    int getDeleteOffersBuy()
    {
        return deleteOffersBuy;
    }

    int getEditOffersSell()
    {
        return editOffersSell;
    }

    int getEditOffersBuy()
    {
        return editOffersBuy;
    }

private:
    int addOffersSell;
    int addOffersBuy;
    int deleteOffersBuy;
    int deleteOffersSell;
    int editOffersBuy;
    int editOffersSell;
};

class CallBackMyOffers : public CallBackDB
{
public:
    CallBackMyOffers() {
        init();
    }

    virtual void finishTableOperation(const TypeTable &table, const TypeTableOperation &operation, const StatusTableOperation &status)
    {
        if (table == MyOffers) {
            if (operation == Add && status == Ok) {
                addOffers++;
            }

            if (operation == Delete && status == Ok) {
                deleteOffers++;
            }

            if (operation == Edit && status == Ok) {
                editOffers++;
            }
        }
    }

    void init()
    {
        addOffers = 0;
        deleteOffers = 0;
        editOffers = 0;
    }

    int getAddOffers()
    {
        return addOffers;
    }

    int getDeleteOffers()
    {
        return deleteOffers;
    }

    int getEditOffers()
    {
        return editOffers;
    }

private:
    int addOffers;
    int deleteOffers;
    int editOffers;
};

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
    CallBackOffers cb;
    cb.init();
    db.setCallBack(&cb);

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

    int step = 0;
    while (true) {
        MilliSleep(100);
        if (cb.getAddOffersBuy() == 3 && cb.getAddOffersSell() == 3) {
            break;
        }

        step++;

        if (step >= 100) {
            BOOST_CHECK(false);
            break;
        }
    }

    BOOST_CHECK(db.isExistOfferBuy(iList.front().idTransaction));
    BOOST_CHECK(db.isExistOfferSell(iList.back().idTransaction));
    BOOST_CHECK(db.isExistOfferBuyByHash(iList.front().hash));
    BOOST_CHECK(db.isExistOfferSellByHash(iList.back().hash));

    BOOST_CHECK(!db.isExistOfferBuy(GetRandHash()));
    BOOST_CHECK(!db.isExistOfferSell(GetRandHash()));
    BOOST_CHECK(!db.isExistOfferBuyByHash(GetRandHash()));
    BOOST_CHECK(!db.isExistOfferSellByHash(GetRandHash()));

    db.deleteOfferSell(iList.front().idTransaction);
    db.deleteOfferBuy(iList.back().idTransaction);

    step = 0;
    while (true) {
        MilliSleep(100);
        if (cb.getDeleteOffersBuy() == 1 && cb.getDeleteOffersSell() == 1) {
            break;
        }

        step++;

        if (step >= 100) {
            BOOST_CHECK(false);
            break;
        }
    }

    BOOST_CHECK(!db.isExistOfferSell(iList.front().idTransaction));
    BOOST_CHECK(!db.isExistOfferBuy(iList.back().idTransaction));

    OfferInfo info1 = iList.back();
    info1.price = 133;
    info1.minAmount = 3;
    info1.shortInfo = "info test";
    info1.countryIso = "AF";
    info1.currencyIso = "AFN";
    info1.paymentMethod = 200;
    info1.timeCreate = 333333;
    info1.timeToExpiration = 3333;

    db.editOfferSell(info1);

    OfferInfo info2 = iList.front();
    info2.price = 4444;
    info2.minAmount = 444;
    info2.shortInfo = "info test 4";
    info2.countryIso = "AX";
    info2.currencyIso = "EUR";
    info2.paymentMethod = 150;
    info2.timeCreate = 4444;
    info2.timeToExpiration = 44444;

    db.editOfferBuy(info2);

    step = 0;
    while (true) {
        MilliSleep(100);
        if (cb.getEditOffersBuy() == 1 && cb.getEditOffersSell() == 1) {
            break;
        }

        step++;

        if (step >= 100) {
            BOOST_CHECK(false);
            break;
        }
    }

    OfferInfo sell = db.getOfferSell(info1.idTransaction);

    BOOST_CHECK(sell.idTransaction == info1.idTransaction);
    BOOST_CHECK(sell.hash == info1.hash);
    BOOST_CHECK(sell.price == info1.price);
    BOOST_CHECK(sell.minAmount == info1.minAmount);
    BOOST_CHECK(sell.shortInfo == info1.shortInfo);
    BOOST_CHECK(sell.countryIso == info1.countryIso);
    BOOST_CHECK(sell.currencyIso == info1.currencyIso);
    BOOST_CHECK(sell.paymentMethod == info1.paymentMethod);
    BOOST_CHECK(sell.timeCreate == info1.timeCreate);
    BOOST_CHECK(sell.timeToExpiration == info1.timeToExpiration);

    sell = db.getOfferSellByHash(info1.hash);

    BOOST_CHECK(sell.idTransaction == info1.idTransaction);
    BOOST_CHECK(sell.hash == info1.hash);
    BOOST_CHECK(sell.price == info1.price);
    BOOST_CHECK(sell.minAmount == info1.minAmount);
    BOOST_CHECK(sell.shortInfo == info1.shortInfo);
    BOOST_CHECK(sell.countryIso == info1.countryIso);
    BOOST_CHECK(sell.currencyIso == info1.currencyIso);
    BOOST_CHECK(sell.paymentMethod == info1.paymentMethod);
    BOOST_CHECK(sell.timeCreate == info1.timeCreate);
    BOOST_CHECK(sell.timeToExpiration == info1.timeToExpiration);

    OfferInfo buy = db.getOfferBuy(info2.idTransaction);

    BOOST_CHECK(buy.idTransaction == info2.idTransaction);
    BOOST_CHECK(buy.hash == info2.hash);
    BOOST_CHECK(buy.price == info2.price);
    BOOST_CHECK(buy.minAmount == info2.minAmount);
    BOOST_CHECK(buy.shortInfo == info2.shortInfo);
    BOOST_CHECK(buy.countryIso == info2.countryIso);
    BOOST_CHECK(buy.currencyIso == info2.currencyIso);
    BOOST_CHECK(buy.paymentMethod == info2.paymentMethod);
    BOOST_CHECK(buy.timeCreate == info2.timeCreate);
    BOOST_CHECK(buy.timeToExpiration == info2.timeToExpiration);

    buy = db.getOfferBuyByHash(info2.hash);

    BOOST_CHECK(buy.idTransaction == info2.idTransaction);
    BOOST_CHECK(buy.hash == info2.hash);
    BOOST_CHECK(buy.price == info2.price);
    BOOST_CHECK(buy.minAmount == info2.minAmount);
    BOOST_CHECK(buy.shortInfo == info2.shortInfo);
    BOOST_CHECK(buy.countryIso == info2.countryIso);
    BOOST_CHECK(buy.currencyIso == info2.currencyIso);
    BOOST_CHECK(buy.paymentMethod == info2.paymentMethod);
    BOOST_CHECK(buy.timeCreate == info2.timeCreate);
    BOOST_CHECK(buy.timeToExpiration == info2.timeToExpiration);

    auto list = db.getOffersSell();

    for (auto item : list) {
        OfferInfo sell = db.getOfferSell(item.idTransaction);

        BOOST_CHECK(sell.idTransaction == item.idTransaction);
        BOOST_CHECK(sell.hash == item.hash);
        BOOST_CHECK(sell.price == item.price);
        BOOST_CHECK(sell.minAmount == item.minAmount);
        BOOST_CHECK(sell.shortInfo == item.shortInfo);
        BOOST_CHECK(sell.countryIso == item.countryIso);
        BOOST_CHECK(sell.currencyIso == item.currencyIso);
        BOOST_CHECK(sell.paymentMethod == item.paymentMethod);
        BOOST_CHECK(sell.timeCreate == item.timeCreate);
        BOOST_CHECK(sell.timeToExpiration == item.timeToExpiration);
    }

    list = db.getOffersBuy();

    for (auto item : list) {
        OfferInfo buy = db.getOfferBuy(item.idTransaction);

        BOOST_CHECK(buy.idTransaction == item.idTransaction);
        BOOST_CHECK(buy.hash == item.hash);
        BOOST_CHECK(buy.price == item.price);
        BOOST_CHECK(buy.minAmount == item.minAmount);
        BOOST_CHECK(buy.shortInfo == item.shortInfo);
        BOOST_CHECK(buy.countryIso == item.countryIso);
        BOOST_CHECK(buy.currencyIso == item.currencyIso);
        BOOST_CHECK(buy.paymentMethod == item.paymentMethod);
        BOOST_CHECK(buy.timeCreate == item.timeCreate);
        BOOST_CHECK(buy.timeToExpiration == item.timeToExpiration);
    }
}

void checkMyOffers(DexDB &db)
{
    CallBackMyOffers cb;
    cb.init();
    db.setCallBack(&cb);

    MyOfferInfo info;
    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.type = Buy;
    info.status = Active;
    info.price = 1234567;
    info.minAmount = 10000;
    info.shortInfo = "first info";
    info.countryIso = "RU";
    info.currencyIso = "RUB";
    info.paymentMethod = 1;
    info.timeCreate = 946688461;
    info.timeToExpiration = 10;

    std::list<MyOfferInfo> iList;
    iList.push_back(info);
    db.addMyOffer(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.type = Buy;
    info.status = Draft;
    info.price = 1555;
    info.minAmount = 0;
    info.shortInfo = "info";
    info.countryIso = "US";
    info.currencyIso = "USD";
    info.paymentMethod = 128;
    info.timeCreate = 996688461;
    info.timeToExpiration = 15;

    iList.push_back(info);
    db.addMyOffer(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.type = Sell;
    info.status = Active;
    info.price = 133;
    info.minAmount = 3;
    info.shortInfo = "info";
    info.countryIso = "UA";
    info.currencyIso = "UAN";
    info.paymentMethod = 128;
    info.timeCreate = 996998461;
    info.timeToExpiration = 9;

    iList.push_back(info);
    db.addMyOffer(info);

    int step = 0;
    while (true) {
        MilliSleep(100);
        if (cb.getAddOffers() == 3) {
            break;
        }

        step++;

        if (step >= 100) {
            BOOST_CHECK(false);
            break;
        }
    }

    BOOST_CHECK(db.isExistMyOffer(iList.front().idTransaction));
    BOOST_CHECK(db.isExistMyOffer(iList.back().idTransaction));

    BOOST_CHECK(!db.isExistMyOffer(GetRandHash()));
    BOOST_CHECK(!db.isExistMyOffer(GetRandHash()));

    db.deleteMyOffer(iList.front().idTransaction);

    step = 0;
    while (true) {
        MilliSleep(100);
        if (cb.getDeleteOffers() == 1) {
            break;
        }

        step++;

        if (step >= 100) {
            BOOST_CHECK(false);
            break;
        }
    }

    BOOST_CHECK(!db.isExistMyOffer(iList.front().idTransaction));

    MyOfferInfo info1 = iList.back();
    info.type = Buy;
    info.status = Expired;
    info1.price = 133;
    info1.minAmount = 3;
    info1.shortInfo = "info test";
    info1.countryIso = "AF";
    info1.currencyIso = "AFN";
    info1.paymentMethod = 77;
    info1.timeCreate = 333333;
    info1.timeToExpiration = 3333;

    db.editMyOffer(info1);

    step = 0;
    while (true) {
        MilliSleep(100);
        if (cb.getEditOffers() == 1) {
            break;
        }

        step++;

        if (step >= 100) {
            BOOST_CHECK(false);
            break;
        }
    }

    MyOfferInfo offer = db.getMyOffer(info1.idTransaction);

    BOOST_CHECK(offer.idTransaction == info1.idTransaction);
    BOOST_CHECK(offer.hash == info1.hash);
    BOOST_CHECK(offer.type == info1.type);
    BOOST_CHECK(offer.status == info1.status);
    BOOST_CHECK(offer.price == info1.price);
    BOOST_CHECK(offer.minAmount == info1.minAmount);
    BOOST_CHECK(offer.shortInfo == info1.shortInfo);
    BOOST_CHECK(offer.countryIso == info1.countryIso);
    BOOST_CHECK(offer.currencyIso == info1.currencyIso);
    BOOST_CHECK(offer.paymentMethod == info1.paymentMethod);
    BOOST_CHECK(offer.timeCreate == info1.timeCreate);
    BOOST_CHECK(offer.timeToExpiration == info1.timeToExpiration);

    auto list = db.getMyOffers();

    for (auto item : list) {
        MyOfferInfo offer = db.getMyOffer(item.idTransaction);

        BOOST_CHECK(offer.idTransaction == item.idTransaction);
        BOOST_CHECK(offer.hash == item.hash);
        BOOST_CHECK(offer.price == item.price);
        BOOST_CHECK(offer.minAmount == item.minAmount);
        BOOST_CHECK(offer.shortInfo == item.shortInfo);
        BOOST_CHECK(offer.countryIso == item.countryIso);
        BOOST_CHECK(offer.currencyIso == item.currencyIso);
        BOOST_CHECK(offer.paymentMethod == item.paymentMethod);
        BOOST_CHECK(offer.timeCreate == item.timeCreate);
        BOOST_CHECK(offer.timeToExpiration == item.timeToExpiration);
    }
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
    checkMyOffers(db);

//    remove(dbName.c_str());
}

BOOST_AUTO_TEST_SUITE_END()

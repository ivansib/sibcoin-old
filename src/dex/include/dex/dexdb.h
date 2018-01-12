#ifndef STORAGE_LEVELDB_INCLUDE_TESTDB_H_
#define STORAGE_LEVELDB_INCLUDE_TESTDB_H_

#include "../../sqlite/sqlite3pp/sqlite3pp.h"
#include "callbackdb.h"
#include <boost/filesystem/path.hpp>
#include <map>
#include "dexdto.h"

namespace dex {

// Please update this DB version number if you change DB schema
const unsigned int uiDexDBversionInCode = 1001;

typedef std::map<CallBackDB*, int> CallBack;

class DexDB {
    DexDB();
    ~DexDB();
    DexDB(const DexDB &) {}
    DexDB &operator =(const DexDB &) {return *this;}

     static DexDB *pSingleton;
     static int nCounter;

public:
    static DexDB *instance();
    static void freeInstance();
    static DexDB *self();

    void addCallBack(CallBackDB *callback);
    CallBackDB *getCallBack() const;
    void removeCallBack(CallBackDB *callBack);

    void addCountry(const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder);
    void editCountries(const std::list<CountryInfo> &list);
    void deleteCountry(const std::string &iso);
    std::list<CountryInfo> getCountriesInfo();
    CountryInfo getCountryInfo(const std::string &iso);

    void addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder);
    void editCurrencies(const std::list<CurrencyInfo> &list);
    void deleteCurrency(const std::string &iso);
    std::list<CurrencyInfo> getCurrenciesInfo();
    CurrencyInfo getCurrencyInfo(const std::string &iso);

    void addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder);
    void deletePaymentMethod(const unsigned char &type);
    std::list<PaymentMethodInfo> getPaymentMethodsInfo();
    PaymentMethodInfo getPaymentMethodInfo(const unsigned char &type);

    void addOfferSell(const OfferInfo &offer);
    void editOfferSell(const OfferInfo &offer);
    void deleteOfferSell(const uint256 &idTransaction);
    void deleteOldOffersSell();
    std::list<OfferInfo> getOffersSell();
    OfferInfo getOfferSell(const uint256 &idTransaction);
    OfferInfo getOfferSellByHash(const uint256 &hash);
    bool isExistOfferSell(const uint256 &idTransaction);
    bool isExistOfferSellByHash(const uint256 &hash);
    std::list<uint256> getSellHashs();

    void addOfferBuy(const OfferInfo &offer);
    void editOfferBuy(const OfferInfo &offer);
    void deleteOfferBuy(const uint256 &idTransaction);
    void deleteOldOffersBuy();
    std::list<OfferInfo> getOffersBuy();
    OfferInfo getOfferBuy(const uint256 &idTransaction);
    OfferInfo getOfferBuyByHash(const uint256 &hash);
    bool isExistOfferBuy(const uint256 &idTransaction);
    bool isExistOfferBuyByHash(const uint256 &hash);
    std::list<uint256> getBuyHashs();

    void addMyOffer(const MyOfferInfo &offer);
    void editMyOffer(const MyOfferInfo &offer);
    void deleteMyOffer(const uint256 &idTransaction);
    void deleteMyOfferByHash(const uint256 &hash);
    void deleteOldMyOffers();
    bool isExistMyOffer(const uint256 &idTransaction);
    bool isExistMyOfferByHash(const uint256 &hash);
    std::list<MyOfferInfo> getMyOffers();
    MyOfferInfo getMyOffer(const uint256 &idTransaction);

    void addFilter(const std::string &filter);
    void deleteFilter(const std::string &filter);
    std::list<std::string> getFilters();

private:
    static void addCountryInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder);
    static void editCountriesInThread(sqlite3pp::database &db, const CallBack &callBack, const std::list<CountryInfo> &list);
    static int editCountryInThread(sqlite3pp::database &db, const std::string &iso, const bool &enabled, const int &sortOrder);
    static void deleteCountryInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso);

    static void addCurrencyInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder);
    static void editCurrenciesInThread(sqlite3pp::database &db, const CallBack &callBack, const std::list<CurrencyInfo> &list);
    static int editCurrencyInThread(sqlite3pp::database &db, const std::string &iso, const bool &enabled, const int &sortOrder);
    static void deleteCurrencyInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso);

    static void addPaymentMethodInThread(sqlite3pp::database &db, const CallBack &callBack, const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder);
    static void editPaymentMethodInThread(sqlite3pp::database &db, const CallBack &callBack, const unsigned char &type, const std::string &name, const std::string &description);
    static void deletePaymentMethodInThread(sqlite3pp::database &db, const CallBack &callBack, const unsigned char &type);

    static void addFilterInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &filter);
    static void deleteFilterInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &filter);

    static void finishTableOperation(const CallBack &callBack, const TypeTable & tables, const TypeTableOperation &operation, const int &status);

    void createTables();
    void addDefaultData();
    int tableCount(const std::string &tableName);
    std::string templateOffersTable(const std::string &tableName) const;

    static void addOffer(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const OfferInfo &offer);
    static void editOffer(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const OfferInfo &offer);
    static void addMyOfferInThread(sqlite3pp::database &db, const CallBack &callBack, const MyOfferInfo &offer);
    static void editMyOfferInThread(sqlite3pp::database &db, const CallBack &callBack, const MyOfferInfo &offer);
    static void deleteOffer(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const uint256 &idTransaction);
    static void deleteOfferByHash(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const uint256 &hash);
    static void deleteOldOffers(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName);
    std::list<OfferInfo> getOffers(const std::string &tableName);
    OfferInfo getOffer(const std::string &tableName, const uint256 &idTransaction);
    OfferInfo getOfferByHash(const std::string &tableName, const uint256 &hash);
    OfferInfo getOffer(const std::string &guery, int &status);
    static int addOrEditOffer(sqlite3pp::database &db, const std::string &query, const OfferInfo &offer);
    static int addOrEditMyOffer(sqlite3pp::database &db, const std::string &query, const MyOfferInfo &offer);
    static void bindOfferData(sqlite3pp::command &cmd, const OfferInfo &offer);
    bool isExistOffer(const std::string &tableName, const uint256 &idTransaction);
    bool isExistOfferByHash(const std::string &tableName, const uint256 &hash);
    std::list<uint256> getHashs(const std::string &tableName);

    sqlite3pp::database db;
    static std::map<CallBackDB*, int> callBack; 

    std::list<CountryInfo> countries;
    std::list<CurrencyInfo> currencies;
    std::list<PaymentMethodInfo> payments;
    bool isGetCountriesDataFromDB;
    bool isGetCurrenciesDataFromDB;
    bool isGetPaymentsDataFromDB;
    void addDbVersion(const int& uiDexDbVersion);
    bool isDexDbOutdated();
    void dropTables();

    void createTestOffers(); // NOTE: for test
};

}

#endif

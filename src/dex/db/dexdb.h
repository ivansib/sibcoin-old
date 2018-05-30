#ifndef STORAGE_LEVELDB_INCLUDE_TESTDB_H_
#define STORAGE_LEVELDB_INCLUDE_TESTDB_H_

#include "../../sqlite/sqlite3pp/sqlite3pp.h"
#include "callbackdb.h"
#include <boost/filesystem/path.hpp>
#include <map>
#include "dexdto.h"

// default options
#define DEFAULT_DEX_VACUUM_ON_STARTUP   1
#define DEFAULT_DEX_BACKUPS             10

namespace dex {

// Please update this DB version number if you change DB schema
const unsigned int uiDexDBversionInCode = 1006;

typedef std::map<CallBackDB*, int> CallBack;

class DexDB {
    DexDB();
    ~DexDB();
    DexDB(const DexDB &) {}
    DexDB &operator =(const DexDB &) {return *this;}

    static DexDB *pSingleton;
    static int nCounter;

public:
    enum class OffersPeriod {
        OldTimeMod,
        YoungTimeMod
    };

    static DexDB *instance();
    static void freeInstance();
    static DexDB *self();
    static bool bOffersRescan;
    static bool AutoBackup (DexDB *db, int nBackups, std::string& strBackupWarning, std::string& strBackupError);

    void addCallBack(CallBackDB *callback);
    CallBackDB *getCallBack() const;
    void removeCallBack(CallBackDB *callBack);
    int backup(sqlite3pp::database &destdb);
    int vacuum();
    sqlite3pp::database *getDB();

    std::string getErrMsg();

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

    void addOfferSell(const OfferInfo &offer, bool usethread = true);
    void editOfferSell(const OfferInfo &offer, bool usethread = true);
    void deleteOfferSell(const uint256 &idTransaction, bool usethread = true);
    void deleteOfferSellByHash(const uint256 &hash, bool usethread = true);
    void deleteOldOffersSell(bool usethread = true);
    std::list<OfferInfo> getOffersSell();
    std::list<OfferInfo> getOffersSell(const OffersPeriod &from, const long long &timeMod);
    std::list<OfferInfo> getOffersSell(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &limit, const int &offset);
    OfferInfo getOfferSell(const uint256 &idTransaction);
    OfferInfo getOfferSellByHash(const uint256 &hash);
    bool isExistOfferSell(const uint256 &idTransaction);
    bool isExistOfferSellByHash(const uint256 &hash);
    std::list<uint256> getSellHashs();
    int countOffersSell();
    int countOffersSell(const OffersPeriod &from, const long long int &timeMod);
    int countOffersSell(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment);
    uint64_t lastModificationOffersSell();

    void addOfferBuy(const OfferInfo &offer, bool usethread = true);
    void editOfferBuy(const OfferInfo &offer, bool usethread = true);
    void deleteOfferBuy(const uint256 &idTransaction, bool usethread = true);
    void deleteOfferBuyByHash(const uint256 &hash, bool usethread = true);
    void deleteOldOffersBuy(bool usethread = true);
    std::list<OfferInfo> getOffersBuy();
    std::list<OfferInfo> getOffersBuy(const OffersPeriod &from, const long long &timeMod);
    std::list<OfferInfo> getOffersBuy(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &limit, const int &offset);
    OfferInfo getOfferBuy(const uint256 &idTransaction);
    OfferInfo getOfferBuyByHash(const uint256 &hash);
    bool isExistOfferBuy(const uint256 &idTransaction);
    bool isExistOfferBuyByHash(const uint256 &hash);
    std::list<uint256> getBuyHashs();
    int countOffersBuy();
    int countOffersBuy(const OffersPeriod &from, const long long int &timeMod);
    int countOffersBuy(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment);
    uint64_t lastModificationOffersBuy();

    void addMyOffer(const MyOfferInfo &offer, bool usethread = true);
    void editMyOffer(const MyOfferInfo &offer, bool usethread = true);
    void deleteMyOffer(const uint256 &idTransaction, bool usethread = true);
    void deleteMyOfferByHash(const uint256 &hash, bool usethread = true);
    void deleteOldMyOffers();
    bool isExistMyOffer(const uint256 &idTransaction);
    bool isExistMyOfferByHash(const uint256 &hash);
    std::list<MyOfferInfo> getMyOffers();
    std::list<MyOfferInfo> getMyOffers(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &type, const int &statusOffer, const int &limit, const int &offset);
    MyOfferInfo getMyOffer(const uint256 &idTransaction);
    MyOfferInfo getMyOfferByHash(const uint256 &hash);
    int countMyOffers();
    int countMyOffers(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &type, const int &statusOffer);
    int setStatusExpiredForMyOffers();

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
    std::list<OfferInfo> getOffers(const std::string &tableName, const OffersPeriod &from, const long long int &timeMod);
    std::list<OfferInfo> getOffers(const std::string &tableName, const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, int const &limit, int const &offset);
    OfferInfo getOffer(const std::string &tableName, const uint256 &idTransaction);
    OfferInfo getOfferByHash(const std::string &tableName, const uint256 &hash);
    OfferInfo getOffer(const std::string &guery, int &status);
    OfferInfo getOffer(sqlite3pp::query::iterator &it);
    MyOfferInfo getMyOffer(sqlite3pp::query::iterator &it);
    static int addOrEditOffer(sqlite3pp::database &db, const std::string &query, const OfferInfo &offer);
    static int addOrEditMyOffer(sqlite3pp::database &db, const std::string &query, const MyOfferInfo &offer);
    static void bindOfferData(sqlite3pp::command &cmd, const OfferInfo &offer);
    bool isExistOffer(const std::string &tableName, const uint256 &idTransaction);
    bool isExistOfferByHash(const std::string &tableName, const uint256 &hash);
    std::list<uint256> getHashs(const std::string &tableName);
    int countOffers(const std::string &tableName, int &status);
    int countOffers(const std::string &tableName, const OffersPeriod &from, const long long int &timeMod, int &status);
    int countOffers(const std::string &tableName, const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &type, const int &statusOffer, int &status);
    uint64_t lastModificationOffers(const std::string &tableName, int &status);

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
    void dropOldTables();
    void renameTables();
    void createIndexes();
    void dropIndexes();
    void moveTablesData();

    void createTestOffers(); // NOTE: for test
};

}

#endif

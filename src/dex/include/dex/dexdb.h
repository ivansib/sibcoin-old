#ifndef STORAGE_LEVELDB_INCLUDE_TESTDB_H_
#define STORAGE_LEVELDB_INCLUDE_TESTDB_H_

#include "../../sqlite/sqlite3pp/sqlite3pp.h"
#include <boost/filesystem/path.hpp>
#include <map>
#include "dexdto.h"

namespace dex {
class DexDB {
public:
    DexDB(const boost::filesystem::path &path);

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
    std::list<OfferInfo> getOffersSell();
    bool isExistOfferSell(const uint256 &idTransaction);

    void addOfferBuy(const OfferInfo &offer);
    void editOfferBuy(const OfferInfo &offer);
    void deleteOfferBuy(const uint256 &idTransaction);
    std::list<OfferInfo> getOffersBuy();
    bool isExistOfferBuy(const uint256 &idTransaction);

    void addMyOffer(const MyOfferInfo &offer);
    void editMyOffer(const MyOfferInfo &offer);
    void deleteMyOffer(const uint256 &idTransaction);
    std::list<MyOfferInfo> getMyOffers();

    void addFilter(const std::string &filter);
    void deleteFilter(const std::string &filter);
    std::list<std::string> getFilters();

private:
    static void addCountryInThread(sqlite3pp::database &db, const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder);
    static void editCountriesInThread(sqlite3pp::database &db, const std::list<CountryInfo> &list);
    static void editCountryInThread(sqlite3pp::database &db, const std::string &iso, const bool &enabled, const int &sortOrder);
    static void deleteCountryInThread(sqlite3pp::database &db, const std::string &iso);

    static void addCurrencyInThread(sqlite3pp::database &db, const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder);
    static void editCurrenciesInThread(sqlite3pp::database &db, const std::list<CurrencyInfo> &list);
    static void editCurrencyInThread(sqlite3pp::database &db, const std::string &iso, const bool &enabled, const int &sortOrder);
    static void deleteCurrencyInThread(sqlite3pp::database &db, const std::string &iso);

    static void addPaymentMethodInThread(sqlite3pp::database &db, const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder);
    static void editPaymentMethodInThread(sqlite3pp::database &db, const unsigned char &type, const std::string &name, const std::string &description);
    static void deletePaymentMethodInThread(sqlite3pp::database &db, const unsigned char &type);

    static void addFilterInThread(sqlite3pp::database &db, const std::string &filter);
    static void deleteFilterInThread(sqlite3pp::database &db, const std::string &filter);

    void createTables();
    void addDefaultData();
    int tableCount(const std::string &tableName);
    std::string templateOffersTable(const std::string &tableName) const;

    static void addOffer(sqlite3pp::database &db, const std::string &tableName, const OfferInfo &offer);
    static void editOffer(sqlite3pp::database &db, const std::string &tableName, const OfferInfo &offer);
    static void deleteOffer(sqlite3pp::database &db, const std::string &tableName, const uint256 &idTransaction);
    std::list<OfferInfo> getOffers(const std::string &tableName);
    static void addOrEditOffer(sqlite3pp::database &db, const std::string &query, const OfferInfo &offer);
    static void addOrEditMyOffer(sqlite3pp::database &db, const std::string &query, const MyOfferInfo &offer);
    static void bindOfferData(sqlite3pp::command &cmd, const OfferInfo &offer);
    bool isExistOffer(const std::string &tableName, const uint256 &idTransaction);

    sqlite3pp::database db;

    std::list<CountryInfo> countries;
    std::list<CurrencyInfo> currencies;
    std::list<PaymentMethodInfo> payments;
    bool isGetCountriesDataFromDB;
    bool isGetCurrenciesDataFromDB;
    bool isGetPaymentsDataFromDB;

    void createTestOffers(); // NOTE: for test
};

}

#endif

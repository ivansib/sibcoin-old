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

    enum TypeView {
        Enabled,
        Disabled,
        All
    };

    void addCountry(const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder);
    void editCountry(const std::string &iso, const bool &enabled, const int &sortOrder);
    void deleteCountry(const std::string &iso);
    std::list<CountryInfo> getCountriesInfo(const TypeView &type = All);
    CountryInfo getCountryInfo(const std::string &iso);

    void addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder);
    void editCurrency(const std::string &iso, const bool &enabled, const int &sortOrder);
    void deleteCurrency(const std::string &iso);
    std::list<CurrencyInfo> getCurrenciesInfo(const TypeView &type = All);
    CurrencyInfo getCurrencyInfo(const std::string &iso);

    void addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder);
    void editPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description);
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

private:
    void createTables();
    void addDefaultData();
    int tableCount(const std::string &tableName);
    std::string templateOffersTable(const std::string &tableName) const;

    void addOffer(const std::string &tableName, const OfferInfo &offer);
    void editOffer(const std::string &tableName, const OfferInfo &offer);
    void deleteOffer(const std::string &tableName, const uint256 &idTransaction);
    std::list<OfferInfo> getOffers(const std::string &tableName);
    void addOrEditOffer(const std::string &query, const OfferInfo &offer);
    void addOrEditMyOffer(const std::string &query, const MyOfferInfo &offer);
    void bindOfferData(sqlite3pp::command &cmd, const OfferInfo &offer);
    bool isExistOffer(const std::string &tableName, const uint256 &idTransaction);

    sqlite3pp::database db;

    void createTestOffers(); // NOTE: for test
};

}

#endif

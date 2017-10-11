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

    void addCountry(const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled = true);
    void editCountry(const std::string &iso, const bool &enabled);
    void deleteCountry(const std::string &iso);
    std::map<std::string, CountryInfo> getCountriesInfo();

    void addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled = true);
    void editCurrency(const std::string &iso, const bool &enabled);
    void deleteCurrency(const std::string &iso);
    std::map<std::string, CurrencyInfo> getCurrenciesInfo();

    void addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description);
    void editPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description);
    void deletePaymentMethod(const unsigned char &type);
    std::map<unsigned char, PaymentMethodInfo> getPaymentMethodsInfo();

    void addOfferSell(const OfferInfo &offer);
    void editOfferSell(const OfferInfo &offer);
    void deleteOfferSell(const int &idTransaction);
    std::list<OfferInfo> getOffersSell();

    void addOfferBuy(const OfferInfo &offer);
    void editOfferBuy(const OfferInfo &offer);
    void deleteOfferBuy(const int &idTransaction);
    std::list<OfferInfo> getOffersBuy();

private:
    void createTables();
    void addDefaultData();
    int tableCount(const std::string &tableName);
    std::string templateOffersTable(const std::string &tableName) const;

    void addOffer(const std::string &tableName, const OfferInfo &offer);
    void editOffer(const std::string &tableName, const OfferInfo &offer);
    void deleteOffer(const std::string &tableName, const int &idTransaction);
    std::list<OfferInfo> getOffers(const std::string &tableName);
    void addOrEditOffer(const std::string &query, const OfferInfo &offer);

    sqlite3pp::database db;

    void createTestOffers(); // NOTE: for test
};

}

#endif

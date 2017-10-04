#ifndef STORAGE_LEVELDB_INCLUDE_TESTDB_H_
#define STORAGE_LEVELDB_INCLUDE_TESTDB_H_

#include "../../sqlite/sqlite3pp/sqlite3pp.h"
#include <boost/filesystem/path.hpp>
#include <map>

namespace dex {

struct CountryInfo {
    std::string name;
    bool enabled;
};

struct CurrencyInfo {
    std::string name;
    bool enabled;
};

class DexDB {
public:
    DexDB(const boost::filesystem::path &path);

    void addCountry(const std::string &iso, const std::string &name, const bool &enabled = true);
    void editCountry(const std::string &iso, const bool &enabled);
    void deleteCountry(const std::string &iso);
    std::map<std::string, CountryInfo> getCountriesInfo();

    void addCurrency(const std::string &iso, const std::string &name, const bool &enabled = true);
    void editCurrency(const std::string &iso, const bool &enabled);
    void deleteCurrency(const std::string &iso);
    std::map<std::string, CurrencyInfo> getCurrenciesInfo();

private:
    void createTables();
    void addDefaultData();
    int tableCount(const std::string &tableName);

    sqlite3pp::database db;
};

}

#endif

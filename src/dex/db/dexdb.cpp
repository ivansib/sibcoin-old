#include <ctime>
#include "dex/dexdb.h"
#include "defaultdatafordb.h"

namespace dex {

DexDB::DexDB(const boost::filesystem::path &path)
{
    std::string dbFile = path.string() + "/dex.db";
    db = sqlite3pp::database(dbFile.c_str());

    createTables();
    addDefaultData();
}

void DexDB::addCountry(const std::string &iso, const std::string &name, const std::string &currency,  const bool &enabled)
{
    sqlite3pp::command cmd(db, "INSERT INTO countries (iso, name, currencyId, enabled) SELECT ?, ?, currencies.id, ? FROM currencies WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, enabled);
    cmd.bind(4, currency, sqlite3pp::nocopy);
    cmd.execute();
}

void DexDB::editCountry(const std::string &iso, const bool &enabled)
{
    sqlite3pp::command cmd(db, "UPDATE countries SET enabled = ? WHERE iso = ?");
    cmd.bind(1, enabled);
    cmd.bind(2, iso, sqlite3pp::nocopy);

    cmd.execute();
}

void DexDB::deleteCountry(const std::string &iso)
{
    sqlite3pp::command cmd(db, "DELETE FROM countries WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    cmd.execute();
}

std::map<std::string, CountryInfo> DexDB::getCountriesInfo()
{
    std::map<std::string, CountryInfo> countries;

    sqlite3pp::query qry(db, "SELECT iso, name, enabled FROM countries");

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        std::string iso;
        std::string name;
        bool enabled;
        std::tie(iso, name, enabled) = (*i).get_columns<std::string, std::string, bool>(0, 1, 2);

        CountryInfo info;
        info.name = name;
        info.enabled = enabled;

        countries[iso] = info;
    }

    return countries;
}

void DexDB::addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled)
{
    sqlite3pp::command cmd(db, "INSERT INTO currencies (iso, name, symbol, enabled) VALUES (?, ?, ?, ?)");
    cmd.bind(1, iso, sqlite3pp::nocopy);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, symbol, sqlite3pp::nocopy);
    cmd.bind(4, enabled);
    cmd.execute();
}

void DexDB::editCurrency(const std::string &iso, const bool &enabled)
{
    sqlite3pp::command cmd(db, "UPDATE currencies SET enabled = ? WHERE iso = ?");
    cmd.bind(1, enabled);
    cmd.bind(2, iso, sqlite3pp::nocopy);

    cmd.execute();
}

void DexDB::deleteCurrency(const std::string &iso)
{
    sqlite3pp::command cmd(db, "DELETE FROM currencies WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    cmd.execute();
}

std::map<std::string, CurrencyInfo> DexDB::getCurrenciesInfo()
{
    std::map<std::string, CurrencyInfo> currencies;

    sqlite3pp::query qry(db, "SELECT iso, name, symbol, enabled FROM currencies");

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        std::string iso;
        std::string name;
        std::string symbol;
        bool enabled;
        std::tie(iso, name, symbol, enabled) = (*i).get_columns<std::string, std::string, std::string, bool>(0, 1, 2, 3);

        CurrencyInfo info;
        info.name = name;
        info.symbol = symbol;
        info.enabled = enabled;

        currencies[iso] = info;
    }

    return currencies;
}

void DexDB::createTables()
{
    db.execute("CREATE TABLE IF NOT EXISTS countries (iso VARCHAR(2) NOT NULL PRIMARY KEY, name VARCHAR(100), enabled BOOLEAN, currencyId INT)");
    db.execute("CREATE TABLE IF NOT EXISTS currencies (id INTEGER PRIMARY KEY, iso VARCHAR(3) UNIQUE, name VARCHAR(100), symbol VARCHAR(10), enabled BOOLEAN)");
}

void DexDB::addDefaultData()
{
    DefaultDataForDb def;

    int count = tableCount("currencies");
    if (count <= 0) {
        std::list<DefaultCurrency> currencies = def.dataCurrencies();

        for (auto item : currencies) {
            addCurrency(item.iso, item.name, item.symbol);
        }
    }

    count = tableCount("countries");
    if (count <= 0) {
        std::list<DefaultCountry> countries = def.dataCountries();

        for (auto item : countries) {
            addCountry(item.iso, item.name, item.currency);
        }
    }
}

int DexDB::tableCount(const std::string &tableName)
{
    int count = 0;

    std::string query = "SELECT count() FROM ";
    query.append(tableName);
    sqlite3pp::query qry(db, query.c_str());

    std::string str;
    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> str;
    count = std::stoi(str);

    return count;
}

}

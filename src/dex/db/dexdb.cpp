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

void DexDB::addCountry(const std::string &iso, const std::string &name, const bool &enabled)
{
    sqlite3pp::command cmd(db, "INSERT INTO countries (iso, name, enabled) VALUES (?, ?, ?)");
    cmd.bind(1, iso, sqlite3pp::nocopy);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, enabled);
    cmd.execute();
}

void dex::DexDB::editCountry(const std::string &iso, const bool &enabled)
{
    sqlite3pp::command cmd(db, "UPDATE countries SET enabled = ? WHERE iso = ?");
    cmd.bind(1, enabled);
    cmd.bind(2, iso, sqlite3pp::nocopy);

    cmd.execute();
}

void dex::DexDB::deleteCountry(const std::string &iso)
{
    sqlite3pp::command cmd(db, "DELETE FROM countries WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    cmd.execute();
}

std::map<std::string, CountryInfo> dex::DexDB::getCountriesInfo()
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

void DexDB::createTables()
{
    db.execute("CREATE TABLE IF NOT EXISTS countries (iso VARCHAR(2) NOT NULL PRIMARY KEY, name VARCHAR(100), enabled BOOLEAN)");
}

void dex::DexDB::addDefaultData()
{
    DefaultDataForDb def;
    int count = tableCount("countries");
    if (count <= 0) {
        std::map<std::string, std::string> countries = def.dataCountries();

        std::map<std::string, std::string>::iterator it = countries.begin();

        while (it != countries.end()) {
            addCountry(it->first, it->second);
            ++it;
        }
    }
}

int dex::DexDB::tableCount(const std::string &tableName)
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

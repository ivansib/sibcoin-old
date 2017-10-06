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


void DexDB::addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description)
{
    sqlite3pp::command cmd(db, "INSERT INTO paymentMethods (type, name, description) VALUES (?, ?, ?)");
    cmd.bind(1, type);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, description, sqlite3pp::nocopy);
    cmd.execute();
}

void DexDB::editPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description)
{
    sqlite3pp::command cmd(db, "UPDATE paymentMethods SET name = ?, description = ? WHERE type = ?");
    cmd.bind(1, name, sqlite3pp::nocopy);
    cmd.bind(2, description, sqlite3pp::nocopy);
    cmd.bind(3, type);

    cmd.execute();
}

void DexDB::deletePaymentMethod(const unsigned char &type)
{
    sqlite3pp::command cmd(db, "DELETE FROM paymentMethods WHERE type = ?");
    cmd.bind(1, type);

    cmd.execute();
}

std::map<unsigned char, PaymentMethodInfo> DexDB::getPaymentMethodsInfo()
{
    std::map<unsigned char, PaymentMethodInfo> payments;

    sqlite3pp::query qry(db, "SELECT type, name, description FROM paymentMethods");

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        unsigned char type;
        std::string name;
        std::string description;
        std::tie(type, name, description) = (*i).get_columns<unsigned char, std::string, std::string>(0, 1, 2);

        PaymentMethodInfo info;
        info.name = name;
        info.description = description;
        payments[type] = info;
    }

    return payments;
}

void DexDB::addOfferSell(const OfferInfo &offer)
{
    addOffer("offersSell", offer);
}

void DexDB::editOfferSell(const OfferInfo &offer)
{
    editOffer("offersSell", offer);
}

void DexDB::deleteOfferSell(const int &idTransaction)
{
    deleteOffer("offersSell", idTransaction);
}

std::list<OfferInfo> DexDB::getOffersSell()
{
    return getOffers("offersSell");
}

void DexDB::addOfferBuy(const OfferInfo &offer)
{
    addOffer("offersBuy", offer);
}

void DexDB::editOfferBuy(const OfferInfo &offer)
{
    editOffer("offersBuy", offer);
}

void DexDB::deleteOfferBuy(const int &idTransaction)
{
    deleteOffer("offersBuy", idTransaction);
}

std::list<OfferInfo> DexDB::getOffersBuy()
{
    return getOffers("offersBuy");
}

void DexDB::addOffer(const std::string &tableName, const OfferInfo &offer)
{
    std::string query = "INSERT INTO " + tableName + " (idTransaction, hash, countryIso, currencyIso, "
                        "paymentMethod, price, minAmount, shortInfo, details) "
                        "VALUES (:idTransaction, :hash, :countryIso, :currencyIso, "
                        ":paymentMethod, :price, :minAmount, :shortInfo, :details)";
    sqlite3pp::command cmd(db, query.c_str());

    cmd.bind(":idTransaction", offer.idTransaction);
    cmd.bind(":hash", static_cast<long long int>(offer.hash));
    cmd.bind(":countryIso", offer.countryIso, sqlite3pp::nocopy);
    cmd.bind(":currencyIso", offer.currencyIso, sqlite3pp::nocopy);
    cmd.bind(":paymentMethod", offer.paymentMethod);
    cmd.bind(":price", static_cast<long long int>(offer.price));
    cmd.bind(":minAmount", static_cast<long long int>(offer.minAmount));
    cmd.bind(":shortInfo", offer.shortInfo, sqlite3pp::nocopy);
    cmd.bind(":details", offer.details, sqlite3pp::nocopy);

    cmd.execute();
}

void DexDB::editOffer(const std::string &tableName, const OfferInfo &offer)
{
    std::string query = "UPDATE " + tableName + " SET hash = :hash, countryIso = :countryIso, currencyIso = :currencyIso, "
                                                "paymentMethod = :paymentMethod, price = :price, minAmount = :minAmount, "
                                                "shortInfo = :shortInfo, details = :details WHERE  idTransaction = :idTransaction";

    sqlite3pp::command cmd(db, query.c_str());

    cmd.bind(":idTransaction", offer.idTransaction);
    cmd.bind(":hash", static_cast<long long int>(offer.hash));
    cmd.bind(":countryIso", offer.countryIso, sqlite3pp::nocopy);
    cmd.bind(":currencyIso", offer.currencyIso, sqlite3pp::nocopy);
    cmd.bind(":paymentMethod", offer.paymentMethod);
    cmd.bind(":price", static_cast<long long int>(offer.price));
    cmd.bind(":minAmount", static_cast<long long int>(offer.minAmount));
    cmd.bind(":shortInfo", offer.shortInfo, sqlite3pp::nocopy);
    cmd.bind(":details", offer.details, sqlite3pp::nocopy);

    cmd.execute();
}

void DexDB::deleteOffer(const std::string &tableName, const int &idTransaction)
{
    std::string query = "DELETE FROM " + tableName + " WHERE idTransaction = ?";

    sqlite3pp::command cmd(db, query.c_str());
    cmd.bind(1, idTransaction);

    cmd.execute();
}

std::list<OfferInfo> DexDB::getOffers(const std::string &tableName)
{
    std::list<OfferInfo> offers;

    std::string str = "SELECT idTransaction, hash, countryIso, currencyIso, "
                      "paymentMethod, price, minAmount, shortInfo, details FROM " + tableName;

    sqlite3pp::query qry(db, str.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        int idTransaction;
        long long int hash;
        std::string countryIso;
        std::string currencyIso;
        uint8_t paymentMethod;
        long long int price;
        long long int minAmount;
        std::string shortInfo;
        std::string details;
        std::tie(idTransaction, hash, countryIso, currencyIso, paymentMethod, price, minAmount, shortInfo, details)
                = (*i).get_columns<int, long long int, std::string, std::string, uint8_t, long long int, long long int, std::string, std::string>
                (0, 1, 2, 3, 4, 5, 6, 7, 8);

        OfferInfo info;
        info.idTransaction = idTransaction;
        info.hash = hash;
        info.countryIso = countryIso;
        info.currencyIso = currencyIso;
        info.paymentMethod = paymentMethod;
        info.price = price;
        info.minAmount = minAmount;
        info.shortInfo = shortInfo;
        info.details = details;
        offers.push_back(info);
    }

    return offers;
}

void DexDB::createTables()
{
    db.execute("CREATE TABLE IF NOT EXISTS countries (iso VARCHAR(2) NOT NULL PRIMARY KEY, name VARCHAR(100), enabled BOOLEAN, currencyId INT)");
    db.execute("CREATE TABLE IF NOT EXISTS currencies (id INTEGER PRIMARY KEY, iso VARCHAR(3) UNIQUE, name VARCHAR(100), symbol VARCHAR(10), enabled BOOLEAN)");
    db.execute("CREATE TABLE IF NOT EXISTS paymentMethods (type TINYINT NOT NULL PRIMARY KEY, name VARCHAR(100), description BLOB)");
    db.execute(templateOffersTable("offersSell").c_str());
    db.execute(templateOffersTable("offersBuy").c_str());
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

    count = tableCount("paymentMethods");
    if (count <= 0) {
        std::list<DefaultPaymentMethod> countries = def.dataPaymentMethods();

        for (auto item : countries) {
            addPaymentMethod(item.type, item.name, item.description);
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

std::string DexDB::templateOffersTable(const std::string &tableName) const
{
    std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (idTransaction INTEGER NOT NULL PRIMARY KEY, "
                        "hash UNSIGNED BIG INT, countryIso VARCHAR(2), "
                        "currencyIso VARCHAR(3), paymentMethod TINYINT, price UNSIGNED BIG INT, "
                        "minAmount UNSIGNED BIG INT, shortInfo VARCHAR(140), details BLOB)";

    return query;
}

}

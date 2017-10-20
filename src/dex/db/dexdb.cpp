#include <ctime>
#include "dex/dexdb.h"
#include "defaultdatafordb.h"

#include "base58.h"
#include "random.h"

namespace dex {

DexDB::DexDB(const boost::filesystem::path &path)
{
    db = sqlite3pp::database(path.c_str());

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

std::list<CountryInfo> DexDB::getCountriesInfo(const TypeView &type)
{
    std::list<CountryInfo> countries;

    std::string query = "SELECT iso, name, enabled FROM countries";

    if (type == Enabled) {
        query += " WHERE enabled = 1";
    } else if (type == Disabled) {
        query += " WHERE enabled = 0";
    }

    sqlite3pp::query qry(db, query.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        std::string iso;
        std::string name;
        bool enabled;
        std::tie(iso, name, enabled) = (*i).get_columns<std::string, std::string, bool>(0, 1, 2);

        CountryInfo info;
        info.iso = iso;
        info.name = name;
        info.enabled = enabled;

        countries.push_back(info);
    }

    return countries;
}

void DexDB::addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO currencies (iso, name, symbol, enabled, sortOrder) VALUES (?, ?, ?, ?, ?)");
    cmd.bind(1, iso, sqlite3pp::nocopy);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, symbol, sqlite3pp::nocopy);
    cmd.bind(4, enabled);
    cmd.bind(5, sortOrder);
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

std::list<CurrencyInfo> DexDB::getCurrenciesInfo(const TypeView &type)
{
    std::list<CurrencyInfo> currencies;

    std::string query = "SELECT iso, name, symbol, enabled FROM currencies";

    if (type == Enabled) {
        query += " WHERE enabled = 1";
    } else if (type == Disabled) {
        query += " WHERE enabled = 0";
    }

    query += " ORDER BY CASE WHEN sortOrder IS '0' THEN '99999' END, sortOrder";

    sqlite3pp::query qry(db, query.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        std::string iso;
        std::string name;
        std::string symbol;
        bool enabled;
        std::tie(iso, name, symbol, enabled) = (*i).get_columns<std::string, std::string, std::string, bool>(0, 1, 2, 3);

        CurrencyInfo info;
        info.iso = iso;
        info.name = name;
        info.symbol = symbol;
        info.enabled = enabled;

        currencies.push_back(info);
    }

    return currencies;
}


void DexDB::addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO paymentMethods (type, name, description, sortOrder) VALUES (?, ?, ?, ?)");
    cmd.bind(1, type);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, description, sqlite3pp::nocopy);
    cmd.bind(4, sortOrder);
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

std::list<PaymentMethodInfo> DexDB::getPaymentMethodsInfo()
{
    std::list<PaymentMethodInfo> payments;

    sqlite3pp::query qry(db, "SELECT type, name, description FROM paymentMethods "
                             "ORDER BY CASE WHEN sortOrder IS '0' THEN '99999' END, sortOrder");

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        unsigned char type;
        std::string name;
        std::string description;
        std::tie(type, name, description) = (*i).get_columns<unsigned char, std::string, std::string>(0, 1, 2);

        PaymentMethodInfo info;
        info.type = type;
        info.name = name;
        info.description = description;
        payments.push_back(info);
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
                        "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details) "
                        "VALUES (:idTransaction, :hash, :countryIso, :currencyIso, "
                        ":paymentMethod, :price, :minAmount, :timeCreate, :timeToExpiration, :shortInfo, :details)";

    addOrEditOffer(query, offer);
}

void DexDB::editOffer(const std::string &tableName, const OfferInfo &offer)
{
    std::string query = "UPDATE " + tableName + " SET hash = :hash, countryIso = :countryIso, currencyIso = :currencyIso, "
                                                "paymentMethod = :paymentMethod, price = :price, minAmount = :minAmount, "
                                                "timeCreate = :timeCreate, timeToExpiration = :timeToExpiration, "
                                                "shortInfo = :shortInfo, details = :details WHERE  idTransaction = :idTransaction";

    addOrEditOffer(query, offer);
}

void dex::DexDB::addOrEditOffer(const std::string &query, const dex::OfferInfo &offer)
{
    sqlite3pp::command cmd(db, query.c_str());

    std::string idTransaction = offer.idTransaction.GetHex();
    std::string hash = offer.hash.GetHex();

    cmd.bind(":idTransaction", idTransaction, sqlite3pp::nocopy);
    cmd.bind(":hash", hash, sqlite3pp::nocopy);
    cmd.bind(":countryIso", offer.countryIso, sqlite3pp::nocopy);
    cmd.bind(":currencyIso", offer.currencyIso, sqlite3pp::nocopy);
    cmd.bind(":paymentMethod", offer.paymentMethod);
    cmd.bind(":price", static_cast<long long int>(offer.price));
    cmd.bind(":minAmount", static_cast<long long int>(offer.minAmount));
    cmd.bind(":timeCreate", static_cast<long long int>(offer.timeCreate));
    cmd.bind(":timeToExpiration", offer.timeToExpiration);
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
                      "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details FROM " + tableName;

    sqlite3pp::query qry(db, str.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        std::string idTransaction;
        std::string hash;
        std::string countryIso;
        std::string currencyIso;
        uint8_t paymentMethod;
        long long int price;
        long long int minAmount;
        long long int timeCreate;
        int timeToExpiration;
        std::string shortInfo;
        std::string details;
        std::tie(idTransaction, hash, countryIso, currencyIso, paymentMethod, price, minAmount,
                 timeCreate, timeToExpiration, shortInfo, details)
                = (*i).get_columns<std::string, std::string, std::string, std::string, uint8_t,
                long long int, long long int, long long int, int, std::string, std::string>
                (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        OfferInfo info;
        info.idTransaction.SetHex(idTransaction);
        info.hash.SetHex(hash);
        info.countryIso = countryIso;
        info.currencyIso = currencyIso;
        info.paymentMethod = paymentMethod;
        info.price = price;
        info.minAmount = minAmount;
        info.shortInfo = shortInfo;
        info.timeCreate = timeCreate;
        info.timeToExpiration = timeToExpiration;
        info.details = details;
        offers.push_back(info);
    }

    return offers;
}

void DexDB::createTables()
{
    db.execute("CREATE TABLE IF NOT EXISTS countries (iso VARCHAR(2) NOT NULL PRIMARY KEY, name VARCHAR(100), enabled BOOLEAN, currencyId INT)");
    db.execute("CREATE TABLE IF NOT EXISTS currencies (id INTEGER PRIMARY KEY, iso VARCHAR(3) UNIQUE, name VARCHAR(100), "
               "symbol VARCHAR(10), enabled BOOLEAN, sortOrder INT)");
    db.execute("CREATE TABLE IF NOT EXISTS paymentMethods (type TINYINT NOT NULL PRIMARY KEY, name VARCHAR(100), description BLOB, sortOrder INT)");
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
            addCurrency(item.iso, item.name, item.symbol, item.enabled, item.sortOrder);
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
            addPaymentMethod(item.type, item.name, item.description, item.sortOrder);
        }
    }

    count = tableCount("offersBuy");
    if (count <= 0) {
        createTestOffers();
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
    std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (idTransaction TEXT NOT NULL PRIMARY KEY, "
                        "hash TEXT, countryIso VARCHAR(2), "
                        "currencyIso VARCHAR(3), paymentMethod TINYINT, price UNSIGNED BIG INT, "
                        "minAmount UNSIGNED BIG INT, timeCreate UNSIGNED BIG INT, timeToExpiration INT, "
                        "shortInfo VARCHAR(140), details TEXT)";

    return query;
}

void dex::DexDB::createTestOffers()
{
    OfferInfo info;

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 1234567;
    info.minAmount = 10000;
    info.shortInfo = "first info";
    info.countryIso = "RU";
    info.currencyIso = "RUB";
    info.paymentMethod = 1;
    info.timeToExpiration = 10;
    addOfferBuy(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 12567;
    info.minAmount = 1000;
    info.shortInfo = "fourt info";
    info.countryIso = "RU";
    info.currencyIso = "RUB";
    info.paymentMethod = 128;
    addOfferBuy(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 345435;
    info.minAmount = 40000;
    info.shortInfo = "second info";
    info.countryIso = "UA";
    info.currencyIso = "UAH";
    info.paymentMethod = 1;
    addOfferBuy(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 567657567;
    info.minAmount = 50000;
    info.shortInfo = "thried info";
    info.countryIso = "US";
    info.currencyIso = "USD";
    info.paymentMethod = 1;
    addOfferBuy(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 432657567;
    info.minAmount = 5000;
    info.shortInfo = "fifth info";
    info.countryIso = "AU";
    info.currencyIso = "AUD";
    info.paymentMethod = 1;
    addOfferBuy(info);
}

}

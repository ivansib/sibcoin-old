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

    if( isDexDbOutdated() ) {
       dropTables();
       createTables();
       addDefaultData();
    }
}

bool DexDB::isDexDbOutdated()
{
   sqlite3pp::query qry(db, "select version from dbversion");
   sqlite3pp::query::iterator i = qry.begin();
   std::string sDBversion;
   std::tie(sDBversion) = (*i).get_columns<std::string>(0);
   unsigned int uiDBversion = std::stoi( sDBversion );

   if( dex::uiDexDBversionInCode != uiDBversion )
      return true;  // dex DB file is is not up to date!
   else
      return false;
}

void DexDB::dropTables()
{
    db.execute("DROP TABLE IF EXISTS dbversion");
    db.execute("DROP TABLE IF EXISTS countries");
    db.execute("DROP TABLE IF EXISTS currencies");
    db.execute("DROP TABLE IF EXISTS paymentMethods");
    db.execute("DROP TABLE IF EXISTS myOffers");
    db.execute("DROP TABLE IF EXISTS offersSell");
    db.execute("DROP TABLE IF EXISTS offersBuy");
}

void DexDB::addCountry(const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder)
{
    countries.clear();

    sqlite3pp::command cmd(db, "INSERT INTO countries (iso, name, currencyId, enabled, sortOrder) SELECT :iso, :name, "
                               "currencies.id, :enabled, :sortOrder FROM currencies WHERE iso = :currencyIso");
    cmd.bind(":iso", iso, sqlite3pp::nocopy);
    cmd.bind(":name", name, sqlite3pp::nocopy);
    cmd.bind(":enabled", enabled);
    cmd.bind(":sortOrder", sortOrder);
    cmd.bind(":currencyIso", currency, sqlite3pp::nocopy);
    cmd.execute();
}

void DexDB::editCountry(const std::string &iso, const bool &enabled, const int &sortOrder)
{
    countries.clear();

    sqlite3pp::command cmd(db, "UPDATE countries SET enabled = :enabled, sortOrder = :sortOrder WHERE iso = :iso");
    cmd.bind(":enabled", enabled);
    cmd.bind(":sortOrder", sortOrder);
    cmd.bind(":iso", iso, sqlite3pp::nocopy);

    cmd.execute();
}

void DexDB::deleteCountry(const std::string &iso)
{
    countries.clear();

    sqlite3pp::command cmd(db, "DELETE FROM countries WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    cmd.execute();
}

std::list<CountryInfo> DexDB::getCountriesInfo(const TypeView &type)
{
    if (countries.empty()) {
        std::string query = "SELECT iso, name, enabled FROM countries";

        if (type == Enabled) {
            query += " WHERE enabled = 1";
        } else if (type == Disabled) {
            query += " WHERE enabled = 0";
        }

        query += " ORDER BY sortOrder";

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
    }

    return countries;
}

CountryInfo DexDB::getCountryInfo(const std::string &iso)
{
    CountryInfo info;

    std::string query = "SELECT name, enabled FROM countries WHERE iso = '" + iso + "'";
    sqlite3pp::query qry(db, query.c_str());

    sqlite3pp::query::iterator i = qry.begin();
    std::string name;
    bool enabled;
    std::tie(name, enabled) = (*i).get_columns<std::string, bool>(0, 1);

    info.iso = iso;
    info.name = name;
    info.enabled = enabled;

    return info;
}

void DexDB::addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder)
{
    currencies.clear();

    sqlite3pp::command cmd(db, "INSERT INTO currencies (iso, name, symbol, enabled, sortOrder) VALUES (?, ?, ?, ?, ?)");
    cmd.bind(1, iso, sqlite3pp::nocopy);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, symbol, sqlite3pp::nocopy);
    cmd.bind(4, enabled);
    cmd.bind(5, sortOrder);
    cmd.execute();
}

void DexDB::editCurrency(const std::string &iso, const bool &enabled, const int &sortOrder)
{
    currencies.clear();

    sqlite3pp::command cmd(db, "UPDATE currencies SET enabled = :enabled, sortOrder = :sortOrder WHERE iso = :iso");
    cmd.bind(":enabled", enabled);
    cmd.bind(":sortOrder", sortOrder);
    cmd.bind(":iso", iso, sqlite3pp::nocopy);

    cmd.execute();
}

void DexDB::deleteCurrency(const std::string &iso)
{
    currencies.clear();

    sqlite3pp::command cmd(db, "DELETE FROM currencies WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    cmd.execute();
}

std::list<CurrencyInfo> DexDB::getCurrenciesInfo(const TypeView &type)
{
    if (currencies.empty()) {
        std::string query = "SELECT iso, name, symbol, enabled FROM currencies";

        if (type == Enabled) {
            query += " WHERE enabled = 1";
        } else if (type == Disabled) {
            query += " WHERE enabled = 0";
        }

        query += " ORDER BY CASE WHEN sortOrder IS '-1' THEN '99999' END, sortOrder";

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
    }

    return currencies;
}

CurrencyInfo DexDB::getCurrencyInfo(const std::string &iso)
{
    CurrencyInfo info;

    std::string query = "SELECT name, symbol, enabled FROM currencies WHERE iso = '" + iso + "'";

    sqlite3pp::query qry(db, query.c_str());

    sqlite3pp::query::iterator i = qry.begin();
    std::string name;
    std::string symbol;
    bool enabled;
    std::tie(name, symbol, enabled) = (*i).get_columns<std::string, std::string, bool>(0, 1, 2);

    info.iso = iso;
    info.name = name;
    info.symbol = symbol;
    info.enabled = enabled;

    return info;
}


void DexDB::addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder)
{
    payments.clear();

    sqlite3pp::command cmd(db, "INSERT INTO paymentMethods (type, name, description, sortOrder) VALUES (?, ?, ?, ?)");
    cmd.bind(1, type);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, description, sqlite3pp::nocopy);
    cmd.bind(4, sortOrder);
    cmd.execute();
}

void DexDB::editPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description)
{
    payments.clear();

    sqlite3pp::command cmd(db, "UPDATE paymentMethods SET name = ?, description = ? WHERE type = ?");
    cmd.bind(1, name, sqlite3pp::nocopy);
    cmd.bind(2, description, sqlite3pp::nocopy);
    cmd.bind(3, type);

    cmd.execute();
}

void DexDB::deletePaymentMethod(const unsigned char &type)
{
    payments.clear();

    sqlite3pp::command cmd(db, "DELETE FROM paymentMethods WHERE type = ?");
    cmd.bind(1, type);

    cmd.execute();
}

std::list<PaymentMethodInfo> DexDB::getPaymentMethodsInfo()
{
    if (payments.empty()) {
        sqlite3pp::query qry(db, "SELECT type, name, description FROM paymentMethods "
                                 "ORDER BY CASE WHEN sortOrder IS '-1' THEN '99999' END, sortOrder");

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
    }

    return payments;
}

PaymentMethodInfo DexDB::getPaymentMethodInfo(const unsigned char &type)
{
    PaymentMethodInfo info;

    sqlite3pp::query qry(db, "SELECT name, description, description FROM paymentMethods WHERE type = ?");
    qry.bind(1, type);

    sqlite3pp::query::iterator i = qry.begin();
    std::string name;
    std::string description;
    std::tie(name, description) = (*i).get_columns<std::string, std::string>(0, 1);

    info.type = type;
    info.name = name;
    info.description = description;

    return info;
}

void DexDB::addOfferSell(const OfferInfo &offer)
{
    addOffer("offersSell", offer);
}

void DexDB::editOfferSell(const OfferInfo &offer)
{
    editOffer("offersSell", offer);
}

void DexDB::deleteOfferSell(const uint256 &idTransaction)
{
    deleteOffer("offersSell", idTransaction);
}

std::list<OfferInfo> DexDB::getOffersSell()
{
    return getOffers("offersSell");
}

bool DexDB::isExistOfferSell(const uint256 &idTransaction)
{
    return isExistOffer("offersSell", idTransaction);
}

void DexDB::addOfferBuy(const OfferInfo &offer)
{
    addOffer("offersBuy", offer);
}

void DexDB::editOfferBuy(const OfferInfo &offer)
{
    editOffer("offersBuy", offer);
}

void DexDB::deleteOfferBuy(const uint256 &idTransaction)
{
    deleteOffer("offersBuy", idTransaction);
}

std::list<OfferInfo> DexDB::getOffersBuy()
{
    return getOffers("offersBuy");
}

bool DexDB::isExistOfferBuy(const uint256 &idTransaction)
{
    return isExistOffer("offersBuy", idTransaction);
}

void DexDB::addMyOffer(const MyOfferInfo &offer)
{
    std::string query = "INSERT INTO myOffers (idTransaction, hash, countryIso, currencyIso, "
                        "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, type, status) "
                        "VALUES (:idTransaction, :hash, :countryIso, :currencyIso, "
                        ":paymentMethod, :price, :minAmount, :timeCreate, :timeToExpiration, :shortInfo, :details, :type, :status)";

    addOrEditMyOffer(query, offer);
}

void DexDB::editMyOffer(const MyOfferInfo &offer)
{
    std::string query = "UPDATE myOffers SET hash = :hash, countryIso = :countryIso, currencyIso = :currencyIso, "
                        "paymentMethod = :paymentMethod, price = :price, minAmount = :minAmount, "
                        "timeCreate = :timeCreate, timeToExpiration = :timeToExpiration, "
                        "shortInfo = :shortInfo, details = :details, "
                        "type = :type, status = :status WHERE  idTransaction = :idTransaction";

    addOrEditMyOffer(query, offer);
}

void DexDB::deleteMyOffer(const uint256 &idTransaction)
{
    deleteOffer("myOffers", idTransaction);
}

std::list<MyOfferInfo> DexDB::getMyOffers()
{
    std::list<MyOfferInfo> offers;

    std::string str = "SELECT idTransaction, hash, countryIso, currencyIso, paymentMethod, price, minAmount, timeCreate, "
                      "timeToExpiration, shortInfo, details, type, status FROM myOffers";

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
        int type;
        int status;
        std::tie(idTransaction, hash, countryIso, currencyIso, paymentMethod, price, minAmount,
                 timeCreate, timeToExpiration, shortInfo, details, type, status)
                = (*i).get_columns<std::string, std::string, std::string, std::string, uint8_t,
                long long int, long long int, long long int, int, std::string, std::string, int, int>
                (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);

        MyOfferInfo info;
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
        info.type = static_cast<TypeOffer>(type);
        info.status = static_cast<StatusOffer>(status);
        offers.push_back(info);
    }

    return offers;
}

void DexDB::addFilter(const std::string &filter)
{
    sqlite3pp::command cmd(db, "INSERT INTO filterList (filter) VALUES (:filter)");
    cmd.bind(":filter", filter, sqlite3pp::nocopy);
    cmd.execute();
}

void DexDB::deleteFilter(const std::string &filter)
{
    sqlite3pp::command cmd(db, "DELETE FROM filterList WHERE filter = :filter");
    cmd.bind(":filter", filter, sqlite3pp::nocopy);

    cmd.execute();
}

std::list<std::string> DexDB::getFilters()
{
    std::list<std::string> filters;

    sqlite3pp::query qry(db, "SELECT filter FROM filterList");

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        std::string item;
        std::tie(item) = (*i).get_columns<std::string>(0);

        filters.push_back(item);
    }

    return filters;
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

void DexDB::addOrEditOffer(const std::string &query, const OfferInfo &offer)
{
    sqlite3pp::command cmd(db, query.c_str());

    bindOfferData(cmd, offer);

    cmd.execute();
}

void DexDB::addOrEditMyOffer(const std::string &query, const MyOfferInfo &offer)
{
    sqlite3pp::command cmd(db, query.c_str());

    bindOfferData(cmd, offer.getOfferInfo());
    cmd.bind(":type", offer.type);
    cmd.bind(":status", offer.status);

    cmd.execute();
}

void DexDB::bindOfferData(sqlite3pp::command &cmd, const OfferInfo &offer)
{
    std::string idTransaction = offer.idTransaction.GetHex();
    std::string hash = offer.hash.GetHex();

    cmd.bind(":idTransaction", idTransaction, sqlite3pp::copy);
    cmd.bind(":hash", hash, sqlite3pp::copy);
    cmd.bind(":countryIso", offer.countryIso, sqlite3pp::nocopy);
    cmd.bind(":currencyIso", offer.currencyIso, sqlite3pp::nocopy);
    cmd.bind(":paymentMethod", offer.paymentMethod);
    cmd.bind(":price", static_cast<long long int>(offer.price));
    cmd.bind(":minAmount", static_cast<long long int>(offer.minAmount));
    cmd.bind(":timeCreate", static_cast<long long int>(offer.timeCreate));
    cmd.bind(":timeToExpiration", offer.timeToExpiration);
    cmd.bind(":shortInfo", offer.shortInfo, sqlite3pp::nocopy);
    cmd.bind(":details", offer.details, sqlite3pp::nocopy);
}

void DexDB::deleteOffer(const std::string &tableName, const uint256 &idTransaction)
{
    std::string query = "DELETE FROM " + tableName + " WHERE idTransaction = ?";

    sqlite3pp::command cmd(db, query.c_str());
    cmd.bind(1, idTransaction.GetHex(), sqlite3pp::copy);

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

bool DexDB::isExistOffer(const std::string &tableName, const uint256 &idTransaction)
{
    int count = 0;

    std::string query = "SELECT count() FROM " + tableName + " WHERE idTransaction = \"" + idTransaction.GetHex() + "\"";
    sqlite3pp::query qry(db, query.c_str());

    std::string str;
    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> str;
    count = std::stoi(str);

    if (count > 0) {
        return true;
    }

    return false;
}

void DexDB::createTables()
{
    db.execute("CREATE TABLE IF NOT EXISTS dbversion (version BIG INT)");
    db.execute("CREATE TABLE IF NOT EXISTS countries (iso VARCHAR(2) NOT NULL PRIMARY KEY, name VARCHAR(100), enabled BOOLEAN, currencyId INT, sortOrder INT)");
    db.execute("CREATE TABLE IF NOT EXISTS currencies (id INTEGER PRIMARY KEY, iso VARCHAR(3) UNIQUE, name VARCHAR(100), "
               "symbol VARCHAR(10), enabled BOOLEAN, sortOrder INT)");
    db.execute("CREATE TABLE IF NOT EXISTS paymentMethods (type TINYINT NOT NULL PRIMARY KEY, name VARCHAR(100), description BLOB, sortOrder INT)");
    db.execute(templateOffersTable("offersSell").c_str());
    db.execute(templateOffersTable("offersBuy").c_str());
    db.execute("CREATE TABLE IF NOT EXISTS myOffers (idTransaction TEXT NOT NULL PRIMARY KEY, "
               "hash TEXT, countryIso VARCHAR(2), "
               "currencyIso VARCHAR(3), paymentMethod TINYINT, price UNSIGNED BIG INT, "
               "minAmount UNSIGNED BIG INT, timeCreate UNSIGNED BIG INT, timeToExpiration INT, "
               "shortInfo VARCHAR(140), details TEXT, type INT, status INT)");

    db.execute("CREATE TABLE IF NOT EXISTS filterList (filter VARCHAR(100) NOT NULL PRIMARY KEY)");
}

void DexDB::addDefaultData()
{
    DefaultDataForDb def;

    int count = tableCount("dbversion");
    if (count <= 0) {
            addDbVersion( dex::uiDexDBversionInCode ); 
    }

    count = tableCount("currencies");
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
            addCountry(item.iso, item.name, item.currency, true, -1);
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

void DexDB::addDbVersion( const int& uiDexDbVersion )
{
   sqlite3pp::command cmd(db, "INSERT INTO dbversion (version) VALUES (?)");
   cmd.bind(1, uiDexDbVersion);
   cmd.execute();
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

void DexDB::createTestOffers()
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

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 1111111;
    info.minAmount = 1000;
    info.shortInfo = "some info";
    info.countryIso = "US";
    info.currencyIso = "USD";
    info.paymentMethod = 1;
    addOfferSell(info);

    MyOfferInfo myInfo;
    myInfo.setOfferInfo(info);
    myInfo.type = Buy;
    myInfo.status = Draft;

    addMyOffer(myInfo);
    getMyOffers();
}

}

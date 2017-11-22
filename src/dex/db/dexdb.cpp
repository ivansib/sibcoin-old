#include <ctime>
#include <thread>
#include "dex/dexdb.h"
#include "defaultdatafordb.h"

#include "base58.h"
#include "random.h"

namespace dex {

CallBackDB *DexDB::callBack = nullptr;

DexDB::DexDB(const boost::filesystem::path &path, CallBackDB *callback)
{
    db = sqlite3pp::database(path.c_str());
    setCallBack(callback);

    isGetCountriesDataFromDB = true;
    isGetCurrenciesDataFromDB = true;
    isGetPaymentsDataFromDB = true;

    createTables();
    addDefaultData();
}

void DexDB::setCallBack(CallBackDB *callBack)
{
    this->callBack = callBack;
}

void DexDB::addCountry(const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder)
{
    countries.push_back({iso, name, enabled});
    std::thread thr(addCountryInThread, std::ref(db), iso, name, currency, enabled, sortOrder);
    thr.detach();
}

void DexDB::editCountries(const std::list<CountryInfo> &list)
{
    if (countries.size() == list.size()) {
        countries = list;
    }

    std::thread thr(editCountriesInThread, std::ref(db), list);
    thr.detach();
}

void DexDB::deleteCountry(const std::string &iso)
{
    countries.remove_if([iso](CountryInfo c){return c.iso == iso;});
    std::thread thr(deleteCountryInThread, std::ref(db), iso);
    thr.detach();
}

std::list<CountryInfo> DexDB::getCountriesInfo()
{
    if (isGetCountriesDataFromDB) {
        std::string query = "SELECT iso, name, enabled FROM countries ORDER BY sortOrder";

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

        isGetCountriesDataFromDB = false;

        int status = qry.finish();
        finishTableOperation(Countries, Read, status);
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

    int status = qry.finish();
    finishTableOperation(Countries, Read, status);

    return info;
}

void DexDB::addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder)
{
    currencies.push_back({iso, name, symbol, enabled});
    std::thread thr(addCurrencyInThread, std::ref(db), iso, name, symbol, enabled, sortOrder);
    thr.detach();
}

void DexDB::editCurrencies(const std::list<CurrencyInfo> &list)
{
    if (currencies.size() == list.size()) {
        currencies = list;
    }
    std::thread thr(editCurrenciesInThread, std::ref(db), list);
    thr.detach();
}

void DexDB::deleteCurrency(const std::string &iso)
{
    currencies.remove_if([iso](CurrencyInfo c){return c.iso == iso;});
    std::thread thr(deleteCurrencyInThread, std::ref(db), iso);
    thr.detach();
}

std::list<CurrencyInfo> DexDB::getCurrenciesInfo()
{
    if (isGetCurrenciesDataFromDB) {
        std::string query = "SELECT iso, name, symbol, enabled FROM currencies ORDER BY sortOrder";

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

        int status = qry.finish();
        finishTableOperation(Currencies, Read, status);

        isGetCurrenciesDataFromDB = false;
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

    int status = qry.finish();
    finishTableOperation(Currencies, Read, status);

    return info;
}

void DexDB::addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder)
{
    payments.push_back({type, name, description});
    std::thread thr(addPaymentMethodInThread, std::ref(db), type, name, description, sortOrder);
    thr.detach();
}

void DexDB::deletePaymentMethod(const unsigned char &type)
{
    payments.remove_if([type](PaymentMethodInfo c){return c.type == type;});
    std::thread thr(deletePaymentMethodInThread, std::ref(db), type);
    thr.detach();
}

std::list<PaymentMethodInfo> DexDB::getPaymentMethodsInfo()
{
    if (isGetPaymentsDataFromDB) {
        sqlite3pp::query qry(db, "SELECT type, name, description FROM paymentMethods ORDER BY sortOrder");

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

        int status = qry.finish();
        finishTableOperation(PaymentMethods, Read, status);

        isGetPaymentsDataFromDB = false;
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

    int status = qry.finish();
    finishTableOperation(PaymentMethods, Read, status);

    return info;
}

void DexDB::addOfferSell(const OfferInfo &offer)
{
    std::thread thr(addOffer, std::ref(db), "offersSell", offer);
    thr.detach();
}

void DexDB::editOfferSell(const OfferInfo &offer)
{
    std::thread thr(editOffer, std::ref(db), "offersSell", offer);
    thr.detach();
}

void DexDB::deleteOfferSell(const uint256 &idTransaction)
{
    std::thread thr(deleteOffer, std::ref(db), "offersSell", idTransaction);
    thr.detach();
}

std::list<OfferInfo> DexDB::getOffersSell()
{
    return getOffers("offersSell");
}

OfferInfo DexDB::getOfferSell(const uint256 &idTransaction)
{
    return getOffer("offersSell", idTransaction);
}

OfferInfo DexDB::getOfferSellByHash(const uint256 &hash)
{
    return getOfferByHash("offersSell", hash);
}

bool DexDB::isExistOfferSell(const uint256 &idTransaction)
{
    return isExistOffer("offersSell", idTransaction);
}

bool DexDB::isExistOfferSellByHash(const uint256 &hash)
{
    return isExistOfferByHash("offersSell", hash);
}

std::list<uint256> DexDB::getSellHashs()
{
    return getHashs("offersSell");
}

void DexDB::addOfferBuy(const OfferInfo &offer)
{
    std::thread thr(addOffer, std::ref(db), "offersBuy", offer);
    thr.detach();
}

void DexDB::editOfferBuy(const OfferInfo &offer)
{
    std::thread thr(editOffer, std::ref(db), "offersBuy", offer);
    thr.detach();
}

void DexDB::deleteOfferBuy(const uint256 &idTransaction)
{
    std::thread thr(deleteOffer, std::ref(db), "offersBuy", idTransaction);
    thr.detach();
}

std::list<OfferInfo> DexDB::getOffersBuy()
{
    return getOffers("offersBuy");
}

OfferInfo DexDB::getOfferBuy(const uint256 &idTransaction)
{
    return getOffer("offersBuy", idTransaction);
}

OfferInfo DexDB::getOfferBuyByHash(const uint256 &hash)
{
    return getOfferByHash("offersBuy", hash);
}

bool DexDB::isExistOfferBuy(const uint256 &idTransaction)
{
    return isExistOffer("offersBuy", idTransaction);
}

bool DexDB::isExistOfferBuyByHash(const uint256 &hash)
{
    return isExistOfferByHash("offersBuy", hash);
}

std::list<uint256> DexDB::getBuyHashs()
{
    return getHashs("offersBuy");
}

void DexDB::addMyOffer(const MyOfferInfo &offer)
{
    std::thread thr(addMyOfferInThread, std::ref(db), offer);
    thr.detach();
}

void DexDB::editMyOffer(const MyOfferInfo &offer)
{
    std::thread thr(editMyOfferInThread, std::ref(db), offer);
    thr.detach();
}

void DexDB::deleteMyOffer(const uint256 &idTransaction)
{
    std::thread thr(deleteOffer, std::ref(db), "myOffers", idTransaction);
    thr.detach();
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

    int status = qry.finish();
    finishTableOperation(MyOffers, Read, status);

    return offers;
}

void DexDB::addFilter(const std::string &filter)
{    
    std::thread thr(addFilterInThread, std::ref(db), filter);
    thr.detach();
}

void DexDB::deleteFilter(const std::string &filter)
{
    std::thread thr(deleteFilterInThread, std::ref(db), filter);
    thr.detach();
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

    int status = qry.finish();
    finishTableOperation(FiltersList, Read, status);

    return filters;
}

void DexDB::addOffer(sqlite3pp::database &db, const std::string &tableName, const OfferInfo &offer)
{
    std::string query = "INSERT INTO " + tableName + " (idTransaction, hash, countryIso, currencyIso, "
                        "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details) "
                        "VALUES (:idTransaction, :hash, :countryIso, :currencyIso, "
                        ":paymentMethod, :price, :minAmount, :timeCreate, :timeToExpiration, :shortInfo, :details)";

    int status = addOrEditOffer(db, query, offer);

    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    }

    finishTableOperation(tTable, Add, status);
}

void DexDB::editOffer(sqlite3pp::database &db, const std::string &tableName, const OfferInfo &offer)
{
    std::string query = "UPDATE " + tableName + " SET hash = :hash, countryIso = :countryIso, currencyIso = :currencyIso, "
                                                "paymentMethod = :paymentMethod, price = :price, minAmount = :minAmount, "
                                                "timeCreate = :timeCreate, timeToExpiration = :timeToExpiration, "
                                                "shortInfo = :shortInfo, details = :details WHERE  idTransaction = :idTransaction";

    int status = addOrEditOffer(db, query, offer);

    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    }

    finishTableOperation(tTable, Edit, status);
}

void DexDB::addMyOfferInThread(sqlite3pp::database &db, const MyOfferInfo &offer)
{
    std::string query = "INSERT INTO myOffers (idTransaction, hash, countryIso, currencyIso, "
                        "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, type, status) "
                        "VALUES (:idTransaction, :hash, :countryIso, :currencyIso, "
                        ":paymentMethod, :price, :minAmount, :timeCreate, :timeToExpiration, :shortInfo, :details, :type, :status)";



    int status = addOrEditMyOffer(db, query, offer);
    finishTableOperation(MyOffers, Add, status);
}

void DexDB::editMyOfferInThread(sqlite3pp::database &db, const MyOfferInfo &offer)
{
    std::string query = "UPDATE myOffers SET hash = :hash, countryIso = :countryIso, currencyIso = :currencyIso, "
                        "paymentMethod = :paymentMethod, price = :price, minAmount = :minAmount, "
                        "timeCreate = :timeCreate, timeToExpiration = :timeToExpiration, "
                        "shortInfo = :shortInfo, details = :details, "
                        "type = :type, status = :status WHERE  idTransaction = :idTransaction";


    int status = addOrEditMyOffer(db, query, offer);
    finishTableOperation(MyOffers, Edit, status);
}


int DexDB::addOrEditOffer(sqlite3pp::database &db, const std::string &query, const OfferInfo &offer)
{
    sqlite3pp::command cmd(db, query.c_str());

    bindOfferData(cmd, offer);

    return cmd.execute();
}

int DexDB::addOrEditMyOffer(sqlite3pp::database &db, const std::string &query, const MyOfferInfo &offer)
{
    sqlite3pp::command cmd(db, query.c_str());

    bindOfferData(cmd, offer.getOfferInfo());
    cmd.bind(":type", offer.type);
    cmd.bind(":status", offer.status);

    return cmd.execute();
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

void DexDB::deleteOffer(sqlite3pp::database &db, const std::string &tableName, const uint256 &idTransaction)
{
    std::string query = "DELETE FROM " + tableName + " WHERE idTransaction = ?";

    sqlite3pp::command cmd(db, query.c_str());
    cmd.bind(1, idTransaction.GetHex(), sqlite3pp::copy);

    int status = cmd.execute();
    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    } else if (tableName == "myOffers") {

    }

    finishTableOperation(tTable, Delete, status);

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

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    int status = qry.finish();
    finishTableOperation(tTable, Read, status);

    return offers;
}

OfferInfo DexDB::getOffer(const std::string &tableName, const uint256 &idTransaction)
{
    std::string str = "SELECT hash, idTransaction, countryIso, currencyIso, "
                      "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details FROM " + tableName
                      + " WHERE idTransaction = \"" + idTransaction.GetHex() + "\"";

    int status;
    OfferInfo info = getOffer(str, status);

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    finishTableOperation(tTable, Read, status);

    return info;
}

OfferInfo DexDB::getOfferByHash(const std::string &tableName, const uint256 &hash)
{
    std::string str = "SELECT hash, idTransaction, countryIso, currencyIso, "
                      "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details FROM " + tableName
                      + " WHERE hash = \"" + hash.GetHex() + "\"";

    int status;
    OfferInfo info = getOffer(str, status);

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    finishTableOperation(tTable, Read, status);

    return info;
}

dex::OfferInfo dex::DexDB::getOffer(const std::string &guery, int &status)
{
    sqlite3pp::query qry(db, guery.c_str());

    sqlite3pp::query::iterator i = qry.begin();

    std::string hash;
    std::string idTransaction;
    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;
    long long int price;
    long long int minAmount;
    long long int timeCreate;
    int timeToExpiration;
    std::string shortInfo;
    std::string details;
    std::tie(hash, idTransaction, countryIso, currencyIso, paymentMethod, price, minAmount,
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

//    TypeTable tTable = OffersBuy;
//    if (tableName == "offersSell") {
//        tTable = OffersSell;
//    }

    status = qry.finish();
//    finishTableOperation(tTable, Read, status);

    return info;
}

bool DexDB::isExistOffer(const std::string &tableName, const uint256 &idTransaction)
{
    int count = 0;

    std::string query = "SELECT count() FROM " + tableName + " WHERE idTransaction = \"" + idTransaction.GetHex() + "\"";
    sqlite3pp::query qry(db, query.c_str());

    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    int status = qry.finish();
    finishTableOperation(tTable, Read, status);

    if (count > 0) {
        return true;
    }

    return false;
}

bool DexDB::isExistOfferByHash(const std::string &tableName, const uint256 &hash)
{
    int count = 0;

    std::string query = "SELECT count() FROM " + tableName + " WHERE hash = \"" + hash.GetHex() + "\"";
    sqlite3pp::query qry(db, query.c_str());

    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    int status = qry.finish();
    finishTableOperation(tTable, Read, status);

    if (count > 0) {
        return true;
    }

    return false;
}

std::list<uint256> DexDB::getHashs(const std::string &tableName)
{
    std::list<uint256> ids;

    std::string query = "SELECT hash FROM " + tableName;
    sqlite3pp::query qry(db, query.c_str());

    for (sqlite3pp::query::iterator it = qry.begin(); it != qry.end(); ++it) {
        std::string strHash;
        (*it).getter() >> strHash;
        uint256 hash;
        hash.SetHex(strHash);

        ids.push_back(hash);
    }

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    int status = qry.finish();
    finishTableOperation(tTable, Read, status);

    return ids;
}

void DexDB::addCountryInThread(sqlite3pp::database &db, const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO countries (iso, name, currencyId, enabled, sortOrder) SELECT :iso, :name, "
                               "currencies.id, :enabled, :sortOrder FROM currencies WHERE iso = :currencyIso");
    cmd.bind(":iso", iso, sqlite3pp::nocopy);
    cmd.bind(":name", name, sqlite3pp::nocopy);
    cmd.bind(":enabled", enabled);
    cmd.bind(":sortOrder", sortOrder);
    cmd.bind(":currencyIso", currency, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(Countries, Add, status);
}

void DexDB::editCountriesInThread(sqlite3pp::database &db, const std::list<CountryInfo> &list)
{
    int sort = 0;
    for (auto item : list) {
        if (editCountryInThread(db, item.iso, item.enabled, sort) == 1) {
            finishTableOperation(Countries, Edit, 1);
            return;
        }
        sort++;
    }

    finishTableOperation(Countries, Edit, 0);
}

int DexDB::editCountryInThread(sqlite3pp::database &db, const std::string &iso, const bool &enabled, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "UPDATE countries SET enabled = :enabled, sortOrder = :sortOrder WHERE iso = :iso");
    cmd.bind(":enabled", enabled);
    cmd.bind(":sortOrder", sortOrder);
    cmd.bind(":iso", iso, sqlite3pp::nocopy);

    if (cmd.execute() == 1) {
        return 1;
    }

    return 0;
}

void DexDB::deleteCountryInThread(sqlite3pp::database &db, const std::string &iso)
{
    sqlite3pp::command cmd(db, "DELETE FROM countries WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(Countries, Delete, status);
}

void DexDB::addCurrencyInThread(sqlite3pp::database &db, const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO currencies (iso, name, symbol, enabled, sortOrder) VALUES (?, ?, ?, ?, ?)");
    cmd.bind(1, iso, sqlite3pp::nocopy);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, symbol, sqlite3pp::nocopy);
    cmd.bind(4, enabled);
    cmd.bind(5, sortOrder);

    int status = cmd.execute();
    finishTableOperation(Countries, Add, status);
}

void DexDB::editCurrenciesInThread(sqlite3pp::database &db, const std::list<CurrencyInfo> &list)
{
    int sort = 0;
    for (auto item : list) {
        if (editCurrencyInThread(db, item.iso, item.enabled, sort) == 1) {
            finishTableOperation(Currencies, Edit, 1);
            return;
        }
        sort++;
    }

    finishTableOperation(Currencies, Edit, 0);
}

int DexDB::editCurrencyInThread(sqlite3pp::database &db, const std::string &iso, const bool &enabled, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "UPDATE currencies SET enabled = :enabled, sortOrder = :sortOrder WHERE iso = :iso");
    cmd.bind(":enabled", enabled);
    cmd.bind(":sortOrder", sortOrder);
    cmd.bind(":iso", iso, sqlite3pp::nocopy);

    if (cmd.execute() == 1) {
        return 1;
    }

    return 0;
}

void DexDB::deleteCurrencyInThread(sqlite3pp::database &db, const std::string &iso)
{
    sqlite3pp::command cmd(db, "DELETE FROM currencies WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(Currencies, Delete, status);
}

void DexDB::addPaymentMethodInThread(sqlite3pp::database &db, const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO paymentMethods (type, name, description, sortOrder) VALUES (?, ?, ?, ?)");
    cmd.bind(1, type);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, description, sqlite3pp::nocopy);
    cmd.bind(4, sortOrder);

    int status = cmd.execute();
    finishTableOperation(PaymentMethods, Add, status);
}

void DexDB::editPaymentMethodInThread(sqlite3pp::database &db, const unsigned char &type, const std::string &name, const std::string &description)
{
    sqlite3pp::command cmd(db, "UPDATE paymentMethods SET name = ?, description = ? WHERE type = ?");
    cmd.bind(1, name, sqlite3pp::nocopy);
    cmd.bind(2, description, sqlite3pp::nocopy);
    cmd.bind(3, type);

    int status = cmd.execute();
    finishTableOperation(PaymentMethods, Edit, status);
}

void DexDB::deletePaymentMethodInThread(sqlite3pp::database &db, const unsigned char &type)
{
    sqlite3pp::command cmd(db, "DELETE FROM paymentMethods WHERE type = ?");
    cmd.bind(1, type);

    int status = cmd.execute();
    finishTableOperation(PaymentMethods, Delete, status);
}

void DexDB::addFilterInThread(sqlite3pp::database &db, const std::string &filter)
{
    sqlite3pp::command cmd(db, "INSERT INTO filterList (filter) VALUES (:filter)");
    cmd.bind(":filter", filter, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(FiltersList, Add, status);
}

void DexDB::deleteFilterInThread(sqlite3pp::database &db, const std::string &filter)
{
    sqlite3pp::command cmd(db, "DELETE FROM filterList WHERE filter = :filter");
    cmd.bind(":filter", filter, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(FiltersList, Delete, status);
}

void DexDB::finishTableOperation(const dex::TypeTable &tables, const dex::TypeTableOperation &operation, const int &status)
{
    if (DexDB::callBack != nullptr) {
        StatusTableOperation s = Ok;
        if (status == 1) {
            s = Error;
        }

        callBack->finishTableOperation(tables, operation, s);
    }
}

void DexDB::createTables()
{
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

    countries.clear();
    currencies.clear();
    payments.clear();

    isGetCountriesDataFromDB = true;
    isGetCurrenciesDataFromDB = true;
    isGetPaymentsDataFromDB = true;

    int count = tableCount("currencies");
    if (count <= 0) {
        std::list<DefaultCurrency> currencies = def.dataCurrencies();
        currencies.sort([](const DefaultCurrency &first, const DefaultCurrency &second) {return first.sortOrder < second.sortOrder;});

        for (auto item : currencies) {
            addCurrency(item.iso, item.name, item.symbol, item.enabled, item.sortOrder);
        }

        isGetCurrenciesDataFromDB = false;
    }

    count = tableCount("countries");
    if (count <= 0) {
        std::list<DefaultCountry> countries = def.dataCountries();
        for (auto item : countries) {
            addCountry(item.iso, item.name, item.currency, true, -1);
        }

        isGetCountriesDataFromDB = false;
    }

    count = tableCount("paymentMethods");
    if (count <= 0) {
        std::list<DefaultPaymentMethod> countries = def.dataPaymentMethods();

        for (auto item : countries) {
            addPaymentMethod(item.type, item.name, item.description, item.sortOrder);
        }

        isGetPaymentsDataFromDB = false;
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

    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;

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

#include <ctime>
#include <thread>
#include "dexdb.h"
#include "defaultdatafordb.h"
#include <boost/thread/thread.hpp>

#include "base58.h"
#include "random.h"
#include "util.h"

namespace dex {

std::map<CallBackDB*, int> DexDB::callBack;
DexDB *DexDB::pSingleton = nullptr;
int DexDB::nCounter = 0;
bool DexDB::bOffersRescan = false;


DexDB::DexDB()
{

    db = sqlite3pp::database(strDexDbFile.c_str(),
            SQLITE_OPEN_READWRITE |
            SQLITE_OPEN_CREATE |
            SQLITE_OPEN_FULLMUTEX |
            SQLITE_OPEN_SHAREDCACHE);

    isGetCountriesDataFromDB = true;
    isGetCurrenciesDataFromDB = true;
    isGetPaymentsDataFromDB = true;

    createTables();
    addDefaultData();

    if (isDexDbOutdated()) {
        dropTables();
        createTables();
        addDefaultData();
    }
}

DexDB::~DexDB()
{
}


DexDB *DexDB::instance()
{
    if (pSingleton == nullptr) {
        pSingleton = new DexDB();
    }

    nCounter++;

    return pSingleton;
}

void DexDB::freeInstance()
{
    if (nCounter > 0) {
        nCounter--;

        if (nCounter == 0) {
            delete pSingleton;
            pSingleton = nullptr;
        }
    }
}

DexDB *DexDB::self()
{
    return pSingleton;
}


void DexDB::addCallBack(CallBackDB *callBack)
{
     this->callBack[callBack]++;
}

void DexDB::removeCallBack(CallBackDB *callBack)
{
    std::map<CallBackDB*,int>::iterator it = this->callBack.find(callBack);

    if (it != this->callBack.end()) {
        if (it->second > 0) {
            it->second--;
            if (it->second == 0) {
                this->callBack.erase(it);
            }
        }
    }
}

bool DexDB::isDexDbOutdated()
{
    sqlite3pp::query qry(db, "SELECT version FROM dbversion");
    sqlite3pp::query::iterator i = qry.begin();

    int iDBversion = 0; 
    std::tie(iDBversion) = (*i).get_columns<int>(0);

    if (dex::uiDexDBversionInCode != (unsigned int)iDBversion)
        return true;  // dex DB version is old!
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
    countries.push_back({iso, name, enabled});    
    boost::thread thr(addCountryInThread, boost::ref(db), boost::ref(callBack), iso, name, currency, enabled, sortOrder);
    thr.detach();
}

void DexDB::editCountries(const std::list<CountryInfo> &list)
{
    if (countries.size() == list.size()) {
        countries = list;
    }

    boost::thread thr(editCountriesInThread, boost::ref(db), boost::ref(callBack), list);
    thr.detach();
}

void DexDB::deleteCountry(const std::string &iso)
{
    countries.remove_if([iso](CountryInfo c){return c.iso == iso;});
    boost::thread thr(deleteCountryInThread, boost::ref(db), boost::ref(callBack), iso);
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
        finishTableOperation(callBack, Countries, Read, status);
    }

    return countries;
}

CountryInfo DexDB::getCountryInfo(const std::string &iso)
{
    CountryInfo info;

    if (isGetCountriesDataFromDB) {
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
        finishTableOperation(callBack, Countries, Read, status);
    } else {
        auto found = std::find_if(countries.begin(), countries.end(), [iso](CountryInfo info){ return info.iso == iso; });
        if (found != countries.end()) {
            info = *found;
        }
    }

    return info;
}

void DexDB::addCurrency(const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder)
{
    currencies.push_back({iso, name, symbol, enabled});
    boost::thread thr(addCurrencyInThread, boost::ref(db), boost::ref(callBack), iso, name, symbol, enabled, sortOrder);
    thr.detach();
}

void DexDB::editCurrencies(const std::list<CurrencyInfo> &list)
{
    if (currencies.size() == list.size()) {
        currencies = list;
    }
    boost::thread thr(editCurrenciesInThread, boost::ref(db), boost::ref(callBack), list);
    thr.detach();
}

void DexDB::deleteCurrency(const std::string &iso)
{
    currencies.remove_if([iso](CurrencyInfo c){return c.iso == iso;});
    boost::thread thr(deleteCurrencyInThread, boost::ref(db), boost::ref(callBack), iso);
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
        finishTableOperation(callBack, Currencies, Read, status);

        isGetCurrenciesDataFromDB = false;
    }

    return currencies;
}

CurrencyInfo DexDB::getCurrencyInfo(const std::string &iso)
{
    CurrencyInfo info;

    if (isGetCurrenciesDataFromDB) {
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
        finishTableOperation(callBack, Currencies, Read, status);
    } else {
        auto found = std::find_if(currencies.begin(), currencies.end(), [iso](CurrencyInfo info){ return info.iso == iso; });
        if (found != currencies.end()) {
            info = *found;
        }
    }

    return info;
}

void DexDB::addPaymentMethod(const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder)
{
    payments.push_back({type, name, description});
    boost::thread thr(addPaymentMethodInThread, boost::ref(db), boost::ref(callBack), type, name, description, sortOrder);
    thr.detach();
}

void DexDB::deletePaymentMethod(const unsigned char &type)
{
    payments.remove_if([type](PaymentMethodInfo c){return c.type == type;});
    boost::thread thr(deletePaymentMethodInThread, boost::ref(db), boost::ref(callBack), type);
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
        finishTableOperation(callBack, PaymentMethods, Read, status);

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
    finishTableOperation(callBack, PaymentMethods, Read, status);

    return info;
}

void DexDB::addOfferSell(const OfferInfo &offer)
{
    boost::thread thr(addOffer, boost::ref(db), boost::ref(callBack), "offersSell", offer);
    thr.detach();
}

void DexDB::editOfferSell(const OfferInfo &offer)
{
    boost::thread thr(editOffer, boost::ref(db), boost::ref(callBack), "offersSell", offer);
    thr.detach();
}

void DexDB::deleteOfferSell(const uint256 &idTransaction)
{
    boost::thread thr(deleteOffer, boost::ref(db), boost::ref(callBack), "offersSell", idTransaction);
    thr.detach();
}

void DexDB::deleteOldOffersSell()
{
    boost::thread thr(deleteOldOffers, boost::ref(db), boost::ref(callBack), "offersSell");
    thr.detach();
}

std::list<OfferInfo> DexDB::getOffersSell()
{
    return getOffers("offersSell");
}

std::list<OfferInfo> DexDB::getOffersSell(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &limit, const int &offset)
{
    return getOffers("offersSell", countryIso, currencyIso, payment, limit, offset);
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

int DexDB::countOffersSell()
{
    std::string tableName = "offersSell";

    int status;
    int count = countOffers(tableName, status);
    finishTableOperation(callBack, OffersSell, Read, status);

    return count;
}

int DexDB::countOffersSell(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment)
{
    std::string tableName = "offersSell";

    int status;
    int count = countOffers(tableName, countryIso, currencyIso, payment, 0, 0, status);
    finishTableOperation(callBack, OffersSell, Read, status);

    return count;
}

void DexDB::addOfferBuy(const OfferInfo &offer)
{
    boost::thread thr(addOffer, boost::ref(db), boost::ref(callBack), "offersBuy", offer);
    thr.detach();
}

void DexDB::editOfferBuy(const OfferInfo &offer)
{
    boost::thread thr(editOffer, boost::ref(db), boost::ref(callBack), "offersBuy", offer);
    thr.detach();
}

void DexDB::deleteOfferBuy(const uint256 &idTransaction)
{
    boost::thread thr(deleteOffer, boost::ref(db), boost::ref(callBack), "offersBuy", idTransaction);
    thr.detach();
}

void DexDB::deleteOldOffersBuy()
{
    boost::thread thr(deleteOldOffers, boost::ref(db), boost::ref(callBack), "offersBuy");
    thr.detach();
}

std::list<OfferInfo> DexDB::getOffersBuy()
{
    return getOffers("offersBuy");
}

std::list<OfferInfo> DexDB::getOffersBuy(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &limit, const int &offset)
{
    return getOffers("offersBuy", countryIso, currencyIso, payment, limit, offset);
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

int DexDB::countOffersBuy()
{
    std::string tableName = "offersBuy";

    int status;
    int count = countOffers(tableName, status);
    finishTableOperation(callBack, OffersBuy, Read, status);

    return count;
}

int DexDB::countOffersBuy(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment)
{
    std::string tableName = "offersBuy";

    int status;
    int count = countOffers(tableName, countryIso, currencyIso, payment, 0, 0, status);
    finishTableOperation(callBack, OffersBuy, Read, status);

    return count;
}

void DexDB::addMyOffer(const MyOfferInfo &offer)
{
    boost::thread thr(addMyOfferInThread, boost::ref(db), boost::ref(callBack), offer);
    thr.detach();
}

void DexDB::editMyOffer(const MyOfferInfo &offer)
{
    boost::thread thr(editMyOfferInThread, boost::ref(db), boost::ref(callBack), offer);
    thr.detach();
}

void DexDB::deleteMyOffer(const uint256 &idTransaction)
{
    boost::thread thr(deleteOffer, boost::ref(db), boost::ref(callBack), "myOffers", idTransaction);
    thr.detach();
}

void DexDB::deleteMyOfferByHash(const uint256 &hash)
{
    boost::thread thr(deleteOfferByHash, boost::ref(db), boost::ref(callBack), "myOffers", hash);
    thr.detach();
}

void DexDB::deleteOldMyOffers()
{
    boost::thread thr(deleteOldOffers, boost::ref(db), boost::ref(callBack), "myOffers");
    thr.detach();
}

bool DexDB::isExistMyOffer(const uint256 &idTransaction)
{
    int count = 0;

    std::string query = "SELECT count() FROM myOffers WHERE idTransaction = \"" + idTransaction.GetHex() + "\"";
    sqlite3pp::query qry(db, query.c_str());

    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;

    int status = qry.finish();
    finishTableOperation(callBack, MyOffers, Read, status);

    if (count > 0) {
        return true;
    }

    return false;
}

bool DexDB::isExistMyOfferByHash(const uint256 &hash)
{
    int count = 0;

    std::string query = "SELECT count() FROM myOffers WHERE hash = \"" + hash.GetHex() + "\"";
    sqlite3pp::query qry(db, query.c_str());

    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;

    int status = qry.finish();
    finishTableOperation(callBack, MyOffers, Read, status);

    if (count > 0) {
        return true;
    }

    return false;
}

std::list<MyOfferInfo> DexDB::getMyOffers()
{
    std::list<MyOfferInfo> offers;

    std::string str = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, paymentMethod, price, minAmount, timeCreate, "
                      "timeToExpiration, shortInfo, details, type, status, editingVersion, editsign FROM myOffers";

    sqlite3pp::query qry(db, str.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        MyOfferInfo info = getMyOffer(i);
        offers.push_back(info);
    }

    int status = qry.finish();
    finishTableOperation(callBack, MyOffers, Read, status);

    return offers;
}

std::list<MyOfferInfo> DexDB::getMyOffers(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &type, const int &statusOffer, const int &limit, const int &offset)
{
    std::list<MyOfferInfo> offers;

    std::string strQuery = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, paymentMethod, price, minAmount, timeCreate, "
                           "timeToExpiration, shortInfo, details, type, status, editingVersion, editsign FROM myOffers";

    std::string where = "";
    if (countryIso != "") {
        where += " countryIso = '" + countryIso + "'";
    }

    if (currencyIso != "") {
        if (where != "") {
            where += " AND";
        }

        where += " currencyIso = '" + currencyIso + "'";
    }

    if (payment > 0) {
        if (where != "") {
            where += " AND";
        }

        where += " paymentMethod = " + std::to_string(payment);
    }

    if (type > 0) {
        if (where != "") {
            where += " AND";
        }

        where += " type = " + std::to_string(type);
    }

    if (statusOffer > 0) {
        if (where != "") {
            where += " AND";
        }

        where += " status = " + std::to_string(statusOffer);
    }

    if (where != "") {
        strQuery += " WHERE" + where;
    }

    if (limit > 0) {
        strQuery += " LIMIT " + std::to_string(limit);

        if (offset > 0) {
            strQuery += " OFFSET " + std::to_string(offset);
        }
    }

    sqlite3pp::query qry(db, strQuery.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        MyOfferInfo info = getMyOffer(i);
        offers.push_back(info);
    }

    int status = qry.finish();
    finishTableOperation(callBack, MyOffers, Read, status);

    return offers;
}

MyOfferInfo DexDB::getMyOffer(const uint256 &idTransaction)
{
    std::string str = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, paymentMethod, price, minAmount, timeCreate, "
                      "timeToExpiration, shortInfo, details, type, status, editingVersion, editsign FROM myOffers WHERE idTransaction = \"" + idTransaction.GetHex() + "\"";

    sqlite3pp::query qry(db, str.c_str());

    sqlite3pp::query::iterator i = qry.begin();
    MyOfferInfo info = getMyOffer(i);

    int stat = qry.finish();
    finishTableOperation(callBack, MyOffers, Read, stat);

    return info;
}

MyOfferInfo DexDB::getMyOfferByHash(const uint256 &hash)
{
    std::string str = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, paymentMethod, price, minAmount, timeCreate, "
                      "timeToExpiration, shortInfo, details, type, status, editingVersion, editsign FROM myOffers WHERE hash = \"" + hash.GetHex() + "\"";

    sqlite3pp::query qry(db, str.c_str());

    sqlite3pp::query::iterator i = qry.begin();
    MyOfferInfo info = getMyOffer(i);

    int stat = qry.finish();
    finishTableOperation(callBack, MyOffers, Read, stat);

    return info;
}

int DexDB::countMyOffers()
{
    std::string tableName = "myOffers";

    int status;
    int count = countOffers(tableName, status);
    finishTableOperation(callBack, MyOffers, Read, status);

    return count;
}

int DexDB::countMyOffers(const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &type, const int &statusOffer)
{
    std::string tableName = "myOffers";

    int status;
    int count = countOffers(tableName, countryIso, currencyIso, payment, type, statusOffer, status);
    finishTableOperation(callBack, MyOffers, Read, status);

    return count;
}

MyOfferInfo DexDB::getMyOffer(sqlite3pp::query::iterator &it)
{
    std::string idTransaction;
    std::string hash;
    std::string pubKey;
    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;
    long long int price;
    long long int minAmount;
    long long int timeCreate;
    long long int timeToExpiration;
    std::string shortInfo;
    std::string details;
    int type;
    int status;
    int editingVersion;
    std::string editsign;
    std::tie(idTransaction, hash, pubKey, countryIso, currencyIso, paymentMethod, price, minAmount,
             timeCreate, timeToExpiration, shortInfo, details, type, status, editingVersion, editsign)
            = (*it).get_columns<std::string, std::string, std::string, std::string, std::string, uint8_t,
            long long int, long long int, long long int, long long int, std::string, std::string, int, int, int, std::string>
            (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    MyOfferInfo info;
    info.idTransaction.SetHex(idTransaction);
    info.hash.SetHex(hash);
    info.pubKey = pubKey;
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
    info.editingVersion = editingVersion;
    info.editsign = editsign;
    return info;
}

void DexDB::addFilter(const std::string &filter)
{
    boost::thread thr(addFilterInThread, boost::ref(db), boost::ref(callBack), filter);
    thr.detach();
}

void DexDB::deleteFilter(const std::string &filter)
{
    boost::thread thr(deleteFilterInThread, boost::ref(db), boost::ref(callBack), filter);
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
    finishTableOperation(callBack, FiltersList, Read, status);

    return filters;
}

void DexDB::addOffer(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const OfferInfo &offer)
{
    std::string query = "INSERT INTO " + tableName + " (idTransaction, hash, pubKey, countryIso, currencyIso, "
                        "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, editingVersion, editsign) "
                        "VALUES (:idTransaction, :hash, :pubKey, :countryIso, :currencyIso, "
                        ":paymentMethod, :price, :minAmount, :timeCreate, :timeToExpiration, :shortInfo, :details, :editingVersion, :editsign)";

    int status = addOrEditOffer(db, query, offer);

    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    }

    finishTableOperation(callBack, tTable, Add, status);
}

void DexDB::editOffer(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const OfferInfo &offer)
{
    std::string query = "UPDATE " + tableName + " SET hash = :hash, pubKey = :pubKey, countryIso = :countryIso, currencyIso = :currencyIso, "
                                                "paymentMethod = :paymentMethod, price = :price, minAmount = :minAmount, "
                                                "timeCreate = :timeCreate, timeToExpiration = :timeToExpiration, "
                                                "shortInfo = :shortInfo, details = :details, editingVersion = :editingVersion, editsign = :editsign WHERE idTransaction = :idTransaction";

    int status = addOrEditOffer(db, query, offer);

    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    }

    finishTableOperation(callBack, tTable, Edit, status);
}

void DexDB::addMyOfferInThread(sqlite3pp::database &db, const CallBack &callBack, const MyOfferInfo &offer)
{
    std::string query = "INSERT INTO myOffers (idTransaction, hash, pubKey, countryIso, currencyIso, "
                        "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, type, status, editingVersion, editsign) "
                        "VALUES (:idTransaction, :hash, :pubKey, :countryIso, :currencyIso, "
                        ":paymentMethod, :price, :minAmount, :timeCreate, :timeToExpiration, :shortInfo, :details, :type, :status, :editingVersion, :editsign)";



    int status = addOrEditMyOffer(db, query, offer);
    finishTableOperation(callBack, MyOffers, Add, status);
}

void DexDB::editMyOfferInThread(sqlite3pp::database &db, const CallBack &callBack, const MyOfferInfo &offer)
{
    std::string query = "UPDATE myOffers SET idTransaction = :idTransaction, countryIso = :countryIso, currencyIso = :currencyIso, "
                        "paymentMethod = :paymentMethod, price = :price, minAmount = :minAmount, "
                        "timeCreate = :timeCreate, timeToExpiration = :timeToExpiration, "
                        "shortInfo = :shortInfo, details = :details, "
                        "type = :type, status = :status, editingVersion = :editingVersion, editsign = :editsign WHERE hash = :hash";


    int status = addOrEditMyOffer(db, query, offer);
    finishTableOperation(callBack, MyOffers, Edit, status);
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
    cmd.bind(":pubKey", offer.pubKey, sqlite3pp::copy);
    cmd.bind(":countryIso", offer.countryIso, sqlite3pp::nocopy);
    cmd.bind(":currencyIso", offer.currencyIso, sqlite3pp::nocopy);
    cmd.bind(":paymentMethod", offer.paymentMethod);
    cmd.bind(":price", static_cast<long long int>(offer.price));
    cmd.bind(":minAmount", static_cast<long long int>(offer.minAmount));
    cmd.bind(":timeCreate", static_cast<long long int>(offer.timeCreate));
    cmd.bind(":timeToExpiration", static_cast<long long int>(offer.timeToExpiration));
    cmd.bind(":shortInfo", offer.shortInfo, sqlite3pp::copy);
    cmd.bind(":details", offer.details, sqlite3pp::copy);
    cmd.bind(":editingVersion", offer.editingVersion);
    cmd.bind(":editsign", offer.editsign, sqlite3pp::copy);
}

void DexDB::deleteOffer(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const uint256 &idTransaction)
{
    std::string query = "DELETE FROM " + tableName + " WHERE idTransaction = ?";

    sqlite3pp::command cmd(db, query.c_str());
    cmd.bind(1, idTransaction.GetHex(), sqlite3pp::copy);

    int status = cmd.execute();
    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    } else if (tableName == "myOffers") {
        tTable = MyOffers;
    }

    finishTableOperation(callBack, tTable, Delete, status);
}

void DexDB::deleteOfferByHash(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName, const uint256 &hash)
{
    std::string query = "DELETE FROM " + tableName + " WHERE hash = ?";

    sqlite3pp::command cmd(db, query.c_str());
    cmd.bind(1, hash.GetHex(), sqlite3pp::copy);

    int status = cmd.execute();
    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    } else if (tableName == "myOffers") {
        tTable = MyOffers;
    }

    finishTableOperation(callBack, tTable, Delete, status);
}

void dex::DexDB::deleteOldOffers(sqlite3pp::database &db, const CallBack &callBack, const std::string &tableName)
{
    std::string query = "DELETE FROM " + tableName + " WHERE timeToExpiration <= :currentTime";

    long long int currentTime = static_cast<long long int>(time(NULL));
    sqlite3pp::command cmd(db, query.c_str());

    cmd.bind(":currentTime", currentTime);

    int status = cmd.execute();
    TypeTable tTable = OffersSell;
    if (tableName == "offersBuy") {
        tTable = OffersBuy;
    } else if (tableName == "myOffers") {
        tTable = MyOffers;
    }

    finishTableOperation(callBack, tTable, Delete, status);
}

std::list<OfferInfo> DexDB::getOffers(const std::string &tableName)
{
    std::list<OfferInfo> offers;

    std::string str = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, "
                      "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, editingVersion, editsign FROM " + tableName;

    sqlite3pp::query qry(db, str.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        OfferInfo info = getOffer(i);
        offers.push_back(info);
    }

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    int status = qry.finish();
    finishTableOperation(callBack, tTable, Read, status);

    return offers;
}

std::list<OfferInfo> DexDB::getOffers(const std::string &tableName, const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &limit, const int &offset)
{
    std::list<OfferInfo> offers;

    std::string strQuery = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, "
                      "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, editingVersion, editsign FROM " + tableName;

    std::string where = "";
    if (countryIso != "") {
        where += " countryIso = '" + countryIso + "'";
    }

    if (currencyIso != "") {
        if (where != "") {
            where += " AND";
        }

        where += " currencyIso = '" + currencyIso + "'";
    }

    if (payment > 0) {
        if (where != "") {
            where += " AND";
        }

        where += " paymentMethod = " + std::to_string(payment);
    }

    if (where != "") {
        strQuery += " WHERE" + where;
    }

    if (limit > 0) {
        strQuery += " LIMIT " + std::to_string(limit);

        if (offset > 0) {
            strQuery += " OFFSET " + std::to_string(offset);
        }
    }

    sqlite3pp::query qry(db, strQuery.c_str());

    for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
        OfferInfo info = getOffer(i);
        offers.push_back(info);
    }

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    int status = qry.finish();
    finishTableOperation(callBack, tTable, Read, status);

    return offers;
}

OfferInfo DexDB::getOffer(const std::string &tableName, const uint256 &idTransaction)
{
    std::string str = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, "
                      "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, editingVersion, editsign FROM " + tableName
                      + " WHERE idTransaction = \"" + idTransaction.GetHex() + "\"";

    int status;
    OfferInfo info = getOffer(str, status);

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    finishTableOperation(callBack, tTable, Read, status);

    return info;
}

OfferInfo DexDB::getOfferByHash(const std::string &tableName, const uint256 &hash)
{
    std::string str = "SELECT idTransaction, hash, pubKey, countryIso, currencyIso, "
                      "paymentMethod, price, minAmount, timeCreate, timeToExpiration, shortInfo, details, editingVersion, editsign FROM " + tableName
                      + " WHERE hash = \"" + hash.GetHex() + "\"";

    int status;
    OfferInfo info = getOffer(str, status);

    TypeTable tTable = OffersBuy;
    if (tableName == "offersSell") {
        tTable = OffersSell;
    }

    finishTableOperation(callBack, tTable, Read, status);

    return info;
}

OfferInfo DexDB::getOffer(const std::string &guery, int &status)
{
    sqlite3pp::query qry(db, guery.c_str());
    sqlite3pp::query::iterator i = qry.begin();
    OfferInfo info = getOffer(i);

    status = qry.finish();

    return info;
}

OfferInfo DexDB::getOffer(sqlite3pp::query::iterator &it)
{
    std::string idTransaction;
    std::string hash;
    std::string pubKey;
    std::string countryIso;
    std::string currencyIso;
    uint8_t paymentMethod;
    long long int price;
    long long int minAmount;
    long long int timeCreate;
    long long int timeToExpiration;
    std::string shortInfo;
    std::string details;
    int editingVersion;
    std::string editsign;
    std::tie(idTransaction, hash, pubKey, countryIso, currencyIso, paymentMethod, price, minAmount,
             timeCreate, timeToExpiration, shortInfo, details, editingVersion, editsign)
            = (*it).get_columns<std::string, std::string, std::string, std::string, std::string, uint8_t,
            long long int, long long int, long long int, long long int, std::string, std::string, int, std::string>
            (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);

    OfferInfo info;
    info.idTransaction.SetHex(idTransaction);
    info.hash.SetHex(hash);
    info.pubKey = pubKey;
    info.countryIso = countryIso;
    info.currencyIso = currencyIso;
    info.paymentMethod = paymentMethod;
    info.price = price;
    info.minAmount = minAmount;
    info.shortInfo = shortInfo;
    info.timeCreate = timeCreate;
    info.timeToExpiration = timeToExpiration;
    info.details = details;
    info.editingVersion = editingVersion;
    info.editsign = editsign;

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
    finishTableOperation(callBack, tTable, Read, status);

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
    finishTableOperation(callBack, tTable, Read, status);

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
    finishTableOperation(callBack, tTable, Read, status);

    return ids;
}

int DexDB::countOffers(const std::string &tableName, int &status)
{
    int count;
    std::string query = "SELECT count(*) FROM " + tableName;
    sqlite3pp::query qry(db, query.c_str());

    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;
    status = qry.finish();

    return count;
}

int DexDB::countOffers(const std::string &tableName, const std::string &countryIso, const std::string &currencyIso, const unsigned char &payment, const int &type, const int &statusOffer, int &status)
{
    int count;
    std::list<MyOfferInfo> offers;

    std::string strQuery = "SELECT count(*) FROM " + tableName;

    std::string where = "";
    if (countryIso != "") {
        where += " countryIso = '" + countryIso + "'";
    }

    if (currencyIso != "") {
        if (where != "") {
            where += " AND";
        }

        where += " currencyIso = '" + currencyIso + "'";
    }

    if (payment > 0) {
        if (where != "") {
            where += " AND";
        }

        where += " paymentMethod = " + std::to_string(payment);
    }

    if (type > 0) {
        if (where != "") {
            where += " AND";
        }

        where += " type = " + std::to_string(type);
    }

    if (statusOffer > 0) {
        if (where != "") {
            where += " AND";
        }

        where += " status = " + std::to_string(statusOffer);
    }

    if (where != "") {
        strQuery += " WHERE" + where;
    }

    sqlite3pp::query qry(db, strQuery.c_str());
    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;
    status = qry.finish();

    return count;
}

void DexDB::addCountryInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso, const std::string &name, const std::string &currency, const bool &enabled, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO countries (iso, name, currencyId, enabled, sortOrder) SELECT :iso, :name, "
                               "currencies.id, :enabled, :sortOrder FROM currencies WHERE iso = :currencyIso");
    cmd.bind(":iso", iso, sqlite3pp::nocopy);
    cmd.bind(":name", name, sqlite3pp::nocopy);
    cmd.bind(":enabled", enabled);
    cmd.bind(":sortOrder", sortOrder);
    cmd.bind(":currencyIso", currency, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(callBack, Countries, Add, status);
}

void DexDB::editCountriesInThread(sqlite3pp::database &db, const CallBack &callBack, const std::list<CountryInfo> &list)
{
    int sort = 0;
    for (auto item : list) {
        if (editCountryInThread(db, item.iso, item.enabled, sort) == 1) {
            finishTableOperation(callBack, Countries, Edit, 1);
            return;
        }
        sort++;
    }

    finishTableOperation(callBack, Countries, Edit, 0);
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

void DexDB::deleteCountryInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso)
{
    sqlite3pp::command cmd(db, "DELETE FROM countries WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(callBack, Countries, Delete, status);
}

void DexDB::addCurrencyInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso, const std::string &name, const std::string &symbol, const bool &enabled, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO currencies (iso, name, symbol, enabled, sortOrder) VALUES (?, ?, ?, ?, ?)");
    cmd.bind(1, iso, sqlite3pp::nocopy);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, symbol, sqlite3pp::nocopy);
    cmd.bind(4, enabled);
    cmd.bind(5, sortOrder);

    int status = cmd.execute();
    finishTableOperation(callBack, Countries, Add, status);
}

void DexDB::editCurrenciesInThread(sqlite3pp::database &db, const CallBack &callBack, const std::list<CurrencyInfo> &list)
{
    int sort = 0;
    for (auto item : list) {
        if (editCurrencyInThread(db, item.iso, item.enabled, sort) == 1) {
            finishTableOperation(callBack, Currencies, Edit, 1);
            return;
        }
        sort++;
    }

    finishTableOperation(callBack, Currencies, Edit, 0);
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

void DexDB::deleteCurrencyInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &iso)
{
    sqlite3pp::command cmd(db, "DELETE FROM currencies WHERE iso = ?");
    cmd.bind(1, iso, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(callBack, Currencies, Delete, status);
}

void DexDB::addPaymentMethodInThread(sqlite3pp::database &db, const CallBack &callBack, const unsigned char &type, const std::string &name, const std::string &description, const int &sortOrder)
{
    sqlite3pp::command cmd(db, "INSERT INTO paymentMethods (type, name, description, sortOrder) VALUES (?, ?, ?, ?)");
    cmd.bind(1, type);
    cmd.bind(2, name, sqlite3pp::nocopy);
    cmd.bind(3, description, sqlite3pp::nocopy);
    cmd.bind(4, sortOrder);

    int status = cmd.execute();
    finishTableOperation(callBack, PaymentMethods, Add, status);
}

void DexDB::editPaymentMethodInThread(sqlite3pp::database &db, const CallBack &callBack, const unsigned char &type, const std::string &name, const std::string &description)
{
    sqlite3pp::command cmd(db, "UPDATE paymentMethods SET name = ?, description = ? WHERE type = ?");
    cmd.bind(1, name, sqlite3pp::nocopy);
    cmd.bind(2, description, sqlite3pp::nocopy);
    cmd.bind(3, type);

    int status = cmd.execute();
    finishTableOperation(callBack, PaymentMethods, Edit, status);
}

void DexDB::deletePaymentMethodInThread(sqlite3pp::database &db, const CallBack &callBack, const unsigned char &type)
{
    sqlite3pp::command cmd(db, "DELETE FROM paymentMethods WHERE type = ?");
    cmd.bind(1, type);

    int status = cmd.execute();
    finishTableOperation(callBack, PaymentMethods, Delete, status);
}

void DexDB::addFilterInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &filter)
{
    sqlite3pp::command cmd(db, "INSERT INTO filterList (filter) VALUES (:filter)");
    cmd.bind(":filter", filter, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(callBack, FiltersList, Add, status);
}

void DexDB::deleteFilterInThread(sqlite3pp::database &db, const CallBack &callBack, const std::string &filter)
{
    sqlite3pp::command cmd(db, "DELETE FROM filterList WHERE filter = :filter");
    cmd.bind(":filter", filter, sqlite3pp::nocopy);

    int status = cmd.execute();
    finishTableOperation(callBack, FiltersList, Delete, status);
}

void DexDB::finishTableOperation(const CallBack &callBack, const dex::TypeTable &tables, const dex::TypeTableOperation &operation, const int &status)
{
    if (callBack.size() > 0) {
        StatusTableOperation s = Ok;
        if (status == 1) {
            s = Error;
        }

        for (auto &item : callBack) {
            if (item.first != nullptr) {
                item.first->finishTableOperation(tables, operation, s);
            }
        }
    }
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
    db.execute("CREATE TABLE IF NOT EXISTS myOffers (hash TEXT NOT NULL PRIMARY KEY, "
               "idTransaction TEXT, pubKey TEXT, countryIso VARCHAR(2), "
               "currencyIso VARCHAR(3), paymentMethod TINYINT, price UNSIGNED BIG INT, "
               "minAmount UNSIGNED BIG INT, timeCreate UNSIGNED BIG INT, timeToExpiration UNSIGNED BIG INT, "
               "shortInfo VARCHAR(140), details TEXT, type INT, status INT, editingVersion INT, editsign VARCHAR(150))");

    db.execute("CREATE TABLE IF NOT EXISTS filterList (filter VARCHAR(100) NOT NULL PRIMARY KEY)");
    bOffersRescan = true;
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

    int count = tableCount("dbversion");
    if (count <= 0) {
        addDbVersion(dex::uiDexDBversionInCode); 
    }

    count = tableCount("currencies");
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

        countries.sort(DefaultCountry::cmp_name);
        countries.sort(DefaultCountry::cmp_sortorder);

        int order = 0;
        for (auto item : countries) {
            addCountry(item.iso, item.name, item.currency, true, order++);
        }

        isGetCountriesDataFromDB = false;
    }

    count = tableCount("paymentMethods");
    if (count <= 0) {
        std::list<DefaultPaymentMethod> methods = def.dataPaymentMethods();

        for (auto item : methods) {
            addPaymentMethod(item.type, item.name, item.description, item.sortOrder);
        }

        isGetPaymentsDataFromDB = false;
    }

//    count = tableCount("offersBuy");
//    if (count <= 0) {
//        createTestOffers();
//    }
}

void DexDB::addDbVersion(const int& uiDexDbVersion)
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

    sqlite3pp::query::iterator it = qry.begin();
    (*it).getter() >> count;

    return count;
}

std::string DexDB::templateOffersTable(const std::string &tableName) const
{
    std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (idTransaction TEXT NOT NULL PRIMARY KEY, "
                        "hash TEXT, pubKey TEXT, countryIso VARCHAR(2), "
                        "currencyIso VARCHAR(3), paymentMethod TINYINT, price UNSIGNED BIG INT, "
                        "minAmount UNSIGNED BIG INT, timeCreate UNSIGNED BIG INT, timeToExpiration UNSIGNED BIG INT, "
                        "shortInfo VARCHAR(140), details TEXT, editingVersion INT, editsign VARCHAR(150))";
    return query;
}

void DexDB::createTestOffers()
{
    OfferInfo info;

    long long int currentTime = static_cast<long long int>(time(NULL));

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 1234567;
    info.minAmount = 10000;
    info.shortInfo = "first info";
    info.countryIso = "RU";
    info.currencyIso = "RUB";
    info.paymentMethod = 1;
    info.timeToExpiration = currentTime + 86400*10;
    addOfferBuy(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 12567;
    info.minAmount = 1000;
    info.shortInfo = "fourt info";
    info.countryIso = "RU";
    info.currencyIso = "RUB";
    info.paymentMethod = 128;
    info.timeToExpiration = currentTime + 86400*20;
    addOfferBuy(info);

    info.hash = GetRandHash();
    info.idTransaction = GetRandHash();
    info.price = 345435;
    info.minAmount = 40000;
    info.shortInfo = "second info";
    info.countryIso = "UA";
    info.currencyIso = "UAH";
    info.paymentMethod = 1;
    info.timeToExpiration = currentTime + 86400*30;
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

#ifndef DEFAULTDATAFORDB_H
#define DEFAULTDATAFORDB_H

#include <list>

namespace dex {

struct DefaultCountry {
    std::string iso;
    std::string name;
    std::string currency;
};

struct DefaultCurrency {
    std::string iso;
    std::string name;
    std::string symbol;
};

class DefaultDataForDb {
public:
    std::list<DefaultCountry> dataCountries();
    std::list<DefaultCurrency> dataCurrencies();
};

}

#endif

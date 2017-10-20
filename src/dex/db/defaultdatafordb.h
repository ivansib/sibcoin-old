#ifndef DEFAULTDATAFORDB_H
#define DEFAULTDATAFORDB_H

#include <list>
#include <string>

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
    bool enabled;
    int sortOrder;

    DefaultCurrency(std::string iso, std::string name, std::string symbol, bool enabled = false, int sortOrder = 0)
    {
        this->iso = iso;
        this->name = name;
        this->symbol = symbol;
        this->enabled = enabled;
        this->sortOrder = sortOrder;
    }
};

struct DefaultPaymentMethod {
    unsigned char type;
    std::string name;
    std::string description;
    int sortOrder;
};

class DefaultDataForDb {
public:
    std::list<DefaultCountry> dataCountries();
    std::list<DefaultCurrency> dataCurrencies();
    std::list<DefaultPaymentMethod> dataPaymentMethods();
};

}

#endif

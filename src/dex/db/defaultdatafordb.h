#ifndef DEFAULTDATAFORDB_H
#define DEFAULTDATAFORDB_H

#include <list>
#include <string>
#include <locale>

namespace dex {

struct DefaultCountry {
    std::string iso;
    std::string name;
    std::string currency;
    int sortOrder;

    DefaultCountry (const std::string &iso, const std::string &name, const std::string &currency, const int sortOrder = 200)
    {
        this->iso = iso;
        this->name = name;
        this->currency = currency;
        this->sortOrder = sortOrder;
    }

    static bool cmp_sortorder(const DefaultCountry &a, const DefaultCountry &b)
    {
      return a.sortOrder < b.sortOrder;
    }

    static bool cmp_name(const DefaultCountry &a, const DefaultCountry &b)
    {
      std::locale loc("");
      return loc(a.name, b.name);
    }

};

struct DefaultCurrency {
    std::string iso;
    std::string name;
    std::string symbol;
    bool enabled;
    int sortOrder;

    DefaultCurrency(std::string iso, std::string name, std::string symbol, bool enabled = false, int sortOrder = 99999)
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

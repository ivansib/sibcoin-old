#ifndef CURRENCYISO_H
#define CURRENCYISO_H

#include <set>
#include <string>

namespace dex {

class CurrencyIso {
public:
    CurrencyIso();

    bool isValid(const std::string &iso);

private:
    void initListIso();
    std::set<std::string> setIso;
};

}

#endif

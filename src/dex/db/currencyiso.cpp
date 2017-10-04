#include "currencyiso.h"
#include "defaultdatafordb.h"

namespace dex {

CurrencyIso::CurrencyIso()
{
    initListIso();
}

bool CurrencyIso::isValid(const std::string &iso)
{
    if (setIso.count(iso) > 0) {
        return true;
    }

    return false;
}

void CurrencyIso::initListIso()
{
    DefaultDataForDb def;

    std::list<DefaultCurrency> currencies = def.dataCurrencies();

    setIso.clear();
    for (auto item : currencies) {
        setIso.insert(item.iso);
    }
}

}

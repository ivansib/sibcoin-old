#include "countryiso.h"
#include "defaultdatafordb.h"

namespace dex {

CountryIso::CountryIso()
{
    initListIso();
}

bool CountryIso::isValid(const std::string &iso)
{
    if (setIso.count(iso) > 0) {
        return true;
    }

    return false;
}

void CountryIso::initListIso()
{
    DefaultDataForDb def;

    std::list<DefaultCountry> countries = def.dataCountries();

    setIso.clear();
    for (auto item : countries) {
        setIso.insert(item.iso);
    }
}

}

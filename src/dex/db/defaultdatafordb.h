#ifndef DEFAULTDATAFORDB_H
#define DEFAULTDATAFORDB_H

#include <map>

namespace dex {

const std::string resourcesPath = "/mnt/D/program/1.project/sibcoin/src/dex/res/";

class DefaultDataForDb {
public:
    std::map<std::string, std::string> dataCountries();
};

}

#endif

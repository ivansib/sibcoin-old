#include <fstream>
#include "defaultdatafordb.h"

namespace dex {

std::map<std::string, std::string> dex::DefaultDataForDb::dataCountries()
{
    std::map<std::string, std::string> countries;
    std::ifstream inFile(resourcesPath + "/country.txt");

    if (inFile) {
        std::string line;
        while (std::getline(inFile, line)) {
            std::string name;
            std::string iso;

            size_t size = line.size();
            name = line.substr(0, size-3);
            iso = line.substr(size - 2 ,2);

            countries[iso] = name;
        }
    }

    return countries;
}

}

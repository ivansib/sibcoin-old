#ifndef PARSERJSONOFFER_H
#define PARSERJSONOFFER_H

#include "dex/db/dexdto.h"
#include <boost/filesystem.hpp>

namespace dex {

MyOfferInfo jsonToMyOfferInfo(const std::string &json, std::string &error);
std::pair<uint64_t, bool> priceFromString(std::string strPrice);
int numberSings(const std::string &str);

int maxOutput();
void changedMaxOutput(const int &max);

void createFileIfNotExist(const boost::filesystem::path &path);

}

#endif

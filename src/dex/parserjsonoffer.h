#ifndef PARSERJSONOFFER_H
#define PARSERJSONOFFER_H

#include "dex/db/dexdto.h"

using namespace dex;

MyOfferInfo jsonToMyOfferInfo(const std::string &json, std::string &error);
uint64_t priceFromString(std::string strPrice);

#endif

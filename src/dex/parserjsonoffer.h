#ifndef PARSERJSONOFFER_H
#define PARSERJSONOFFER_H

#include "dex/dexdto.h"

using namespace dex;

MyOfferInfo fromJsonForAdd(const std::string &json, std::string &error);

#endif

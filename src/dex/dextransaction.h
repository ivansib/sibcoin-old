#ifndef __DEX_TRANSACTION_H__
#define __DEX_TRANSACTION_H__



#include "key.h"
#include "main.h"
#include "net.h"
#include "dex/dexdto.h"
#include "dex.h"




bool CreatePayOfferTransaction(std::vector<CTxIn> txin, CDexOffer &offer, const std::string &changeaddress);





#endif //__DEX_TRANSACTION_H__


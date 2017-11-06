#ifndef __DEX_TRANSACTION_H__
#define __DEX_TRANSACTION_H__



#include "key.h"
#include "main.h"
#include "net.h"
#include "dex/dexdto.h"
#include "dex.h"




CMutableTransaction CreatePayOfferTransaction(std::vector<CTxIn> txin, CDexOffer &offer, std::string &sError, const std::string &changeaddress);
bool SignPayOfferTransaction(CMutableTransaction &Tx, std::vector<std::string> privkeys, std::string &sError);
bool RelayPayOfferTransaction(CMutableTransaction &Tx, std::string &sError);



#endif //__DEX_TRANSACTION_H__


#ifndef __DEX_TRANSACTION_H__
#define __DEX_TRANSACTION_H__

#include "primitives/transaction.h"
#include "dex.h"

namespace dex {
bool CreatePayOfferTransaction(const CDexOffer &offer, CTransaction &newTx, std::string &sError);
}
#endif //__DEX_TRANSACTION_H__


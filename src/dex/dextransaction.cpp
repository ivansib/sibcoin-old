#include "dextransaction.h"

#include "init.h"
#include "util.h"
#include "utilstrencodings.h"

#include "dexoffer.h"
#include "dex/dexdb.h"
#include "txmempool.h"
#include "base58.h"




#define PAYOFFER_RETURN_FEE     10000
#define PAYOFFER_TX_FEE         50000000
#define PAYOFFER_MIN_TX_HEIGHT  6




#define CHECK(A,B) { if (!(A)) { LogPrintf("%s %s %d: %s", __FILE__, __FUNCTION__, __LINE__, std::string(B).c_str()); return false; } }




bool CreatePayOfferTransaction(std::vector<CTxIn> txin, CDexOffer &offer, const std::string &changeaddress)
{
    CMutableTransaction Tx;
    LOCK(cs_main);
    
    CAmount totalin = 0;

    for (auto i : txin) {
        CCoins coins;

        CHECK(pcoinsTip->GetCoins(i.prevout.hash, coins), "not GetCoins");

        if (i.prevout.n<0 || (unsigned int)(i.prevout.n)>=coins.vout.size() || coins.vout[i.prevout.n].IsNull())
            return false;

        if ((unsigned int)coins.nHeight == MEMPOOL_HEIGHT)
            return false;

        BlockMap::iterator it = mapBlockIndex.find(pcoinsTip->GetBestBlock());
        CBlockIndex *pindex = it->second;
        if ((pindex->nHeight - coins.nHeight + 1) < PAYOFFER_MIN_TX_HEIGHT)
            return false;

        totalin += coins.vout[i.prevout.n].nValue;

        Tx.vin.push_back(i);
    }

    CHECK(totalin >= (PAYOFFER_RETURN_FEE + PAYOFFER_TX_FEE), "not enough money");


    /// offer fee out
    CTxOut out(PAYOFFER_RETURN_FEE, CScript() << OP_RETURN << ParseHex(offer.MakeHash().GetHex()));
    Tx.vout.push_back(out);

    /// change out
    CAmount nAmount = totalin - (PAYOFFER_RETURN_FEE + PAYOFFER_TX_FEE);
    if (nAmount > 0) { /// add change out to transaction
        CBitcoinAddress address(changeaddress);
        CHECK(address.IsValid(), std::string("Invalid Sibcoin address: ")+changeaddress);

        CScript scriptPubKey = GetScriptForDestination(address.Get());
        CTxOut out(nAmount, scriptPubKey);
        Tx.vout.push_back(out);
    }


    return true;
}






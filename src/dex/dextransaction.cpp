
#include "init.h"
#include "util.h"
#include "utilstrencodings.h"
#include "txmempool.h"
#include "base58.h"
#include "script/sign.h"
#include "script/standard.h"
#include "policy/policy.h"
#include "consensus/validation.h"
#include "core_io.h"
#include "coincontrol.h"

#include "dextransaction.h"
#include "dexoffer.h"
#include "dex/db/dexdb.h"
#include "dex.h"

#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif


#define CHECK(A,B,...) { if (!(A)) { std::string str = strprintf(std::string("%s") + (B), "",  ##__VA_ARGS__); LogPrintf("%s:%d: %s\n", __FILE__, __LINE__, str.c_str()); sError += str; break; } }




bool CreatePayOfferTransaction(const CDexOffer &offer, CTransaction &newTx, std::string &sError)
{
#ifdef ENABLE_WALLET
    do {
        CAmount curBalance = pwalletMain->GetBalance();
        int days = ((offer.timeExpiration - offer.timeCreate - 1) / 86400) + 1;
        int coef = ((days - 1) / 10) + 1;

        CHECK(curBalance >= (PAYOFFER_RETURN_FEE + PAYOFFER_TX_FEE * coef), "Insufficient funds");

        CScript scriptPubKey = CScript() << OP_RETURN << ParseHex(offer.hash.GetHex());

        CReserveKey reservekey(pwalletMain);
        CWalletTx wtxNew;
        CAmount nFeeRequired;
        std::string strError;
        std::vector<CRecipient> vecSend;
        int nChangePosRet = -1;
        CRecipient recipient = {scriptPubKey, PAYOFFER_RETURN_FEE, false};
        vecSend.push_back(recipient);
        CCoinControl ccoin;
        ccoin.nMinimumTotalFee = PAYOFFER_TX_FEE * coef;
        CHECK(pwalletMain->CreateTransaction(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strError, &ccoin), strError);
        CHECK(pwalletMain->CommitTransaction(wtxNew, reservekey), "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
        newTx = wtxNew;
        return true;
    } while(false);
#endif
    return false;
}



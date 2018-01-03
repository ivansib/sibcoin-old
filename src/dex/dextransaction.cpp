#include "dextransaction.h"

#include "init.h"
#include "util.h"
#include "utilstrencodings.h"

#include "dexoffer.h"
#include "dex/dexdb.h"
#include "txmempool.h"
#include "base58.h"
#include "script/sign.h"
#include "policy/policy.h"
#include "consensus/validation.h"
#include "core_io.h"
#include "dex.h"

#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif


CAmount dexPayTxFee = 0;

#define CHECK(A,B,...) { if (!(A)) { std::string str = strprintf(std::string("%s") + (B), "",  ##__VA_ARGS__); LogPrintf("%s:%d: %s\n", __FILE__, __LINE__, str.c_str()); sError += str; break; } }




bool CreatePayOfferTransaction(const CDexOffer &offer, CTransaction &newTx, std::string &sError)
{
#ifdef ENABLE_WALLET
    do {
        CAmount curBalance = pwalletMain->GetBalance();

        int coef = offer.timeExpiration / 10;

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
        dexPayTxFee = PAYOFFER_TX_FEE * coef;
        CHECK(pwalletMain->CreateTransaction(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strError), strError);
        CHECK(pwalletMain->CommitTransaction(wtxNew, reservekey), "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
        newTx = wtxNew;
        return true;
    } while(false);
#endif
    dexPayTxFee = 0;
    return false;
}



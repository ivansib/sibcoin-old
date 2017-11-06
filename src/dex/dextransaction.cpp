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

#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif





#define PAYOFFER_RETURN_FEE     10000
#define PAYOFFER_TX_FEE         50000000
#define PAYOFFER_MIN_TX_HEIGHT  6




#define CHECK(A,B,...) { if (!(A)) { sError += strprintf(std::string("%s(%s):%d  ")+ (B) +std::string("\n"), __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); break; } }




CMutableTransaction CreatePayOfferTransaction(std::vector<CTxIn> txin, CDexOffer &offer, std::string &sError, const std::string &changeaddress)
{
    CMutableTransaction Tx;
    CAmount totalin = 0;
    do {
        LOCK(cs_main);
        CHECK(!offer.isNull(), "dex offer is empty");
        /// add txin
        for (auto i : txin) {
            CCoins coins;

            CHECK((pcoinsTip->GetCoins(i.prevout.hash, coins)), "GetCoins error");

            CHECK((unsigned int)(i.prevout.n) < coins.vout.size() &&  !coins.vout[i.prevout.n].IsNull(), "txin error");

            CHECK((unsigned int)coins.nHeight != MEMPOOL_HEIGHT, "txin height error");

            BlockMap::iterator it = mapBlockIndex.find(pcoinsTip->GetBestBlock());
            CBlockIndex *pindex = it->second;
            CHECK((pindex->nHeight - coins.nHeight + 1) >= PAYOFFER_MIN_TX_HEIGHT, "txin height error");

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
            CHECK(address.IsValid(), "Invalid Sibcoin address: %s", changeaddress.c_str());

            CScript scriptPubKey = GetScriptForDestination(address.Get());
            CTxOut out(nAmount, scriptPubKey);
            Tx.vout.push_back(out);
        }
    } while(false);

    return Tx;
}



bool SignPayOfferTransaction(CMutableTransaction &Tx, std::vector<std::string> privkeys, std::string &sError) {
    do {
        CHECK(Tx.vin.size() > 0, "Missing transaction in");
        CHECK(Tx.vout.size() > 0, "Missing transaction out");


#ifdef ENABLE_WALLET
        LOCK2(cs_main, pwalletMain ? &pwalletMain->cs_wallet : NULL);
#else
        LOCK(cs_main);
#endif



        // Fetch previous transactions (inputs):
        CCoinsView viewDummy;
        CCoinsViewCache view(&viewDummy);
        {
            LOCK(mempool.cs);
            CCoinsViewCache &viewChain = *pcoinsTip;
            CCoinsViewMemPool viewMempool(&viewChain, mempool);
            view.SetBackend(viewMempool); // temporarily switch cache backend to db+mempool view

            BOOST_FOREACH(const CTxIn& txin, Tx.vin) {
                const uint256& prevHash = txin.prevout.hash;
                CCoins coins;
                view.AccessCoins(prevHash); // this is certainly allowed to fail
            }

            view.SetBackend(viewDummy); // switch back to avoid locking mempool for too long
        }


        bool fGivenKeys = false;
        CBasicKeyStore tempKeystore;
        if (privkeys.size() > 0) {
            fGivenKeys = true;
            for (unsigned int idx = 0; idx < privkeys.size(); idx++) {
                CBitcoinSecret vchSecret;
                bool fGood = vchSecret.SetString(privkeys[idx]);
                CHECK(fGood, "Invalid private key");
                CKey key = vchSecret.GetKey();
                CHECK(key.IsValid(), "Private key outside allowed range");
                tempKeystore.AddKey(key);
            }
        }
#ifdef ENABLE_WALLET
        else if (pwalletMain) {
            CHECK(!pwalletMain->IsLocked(), "Error: Wallet is locked");
        }
#endif

#ifdef ENABLE_WALLET
        const CKeyStore& keystore = ((fGivenKeys || !pwalletMain) ? tempKeystore : *pwalletMain);
#else
        const CKeyStore& keystore = tempKeystore;
#endif

        for (unsigned int i = 0; i < Tx.vin.size(); i++) {
            CTxIn& txin = Tx.vin[i];
            const CCoins* coins = view.AccessCoins(txin.prevout.hash);
            CHECK(coins != NULL && coins->IsAvailable(txin.prevout.n), "Input not found or already spent");

            const CScript& prevPubKey = coins->vout[txin.prevout.n].scriptPubKey;

            txin.scriptSig.clear();
            SignSignature(keystore, prevPubKey, Tx, i);

            ScriptError serror = SCRIPT_ERR_OK;
            CHECK(VerifyScript(txin.scriptSig, prevPubKey,
                              STANDARD_SCRIPT_VERIFY_FLAGS, MutableTransactionSignatureChecker(&Tx, i), &serror),
                              "verify transaction error");
        }

    } while(false);
    return true;
}





bool RelayPayOfferTransaction(CMutableTransaction &Tx, std::string &sError)
{
    LOCK(cs_main);

    CTransaction tx(Tx);
    uint256 hashTx = tx.GetHash();

    ///bool fInstantSend = false;

    CCoinsViewCache &view = *pcoinsTip;
    const CCoins* existingCoins = view.AccessCoins(hashTx);
    bool fHaveMempool = mempool.exists(hashTx);
    bool fHaveChain = existingCoins && existingCoins->nHeight < 1000000000;
    do {
        if (!fHaveMempool && !fHaveChain) {
            // push to local node and sync with wallets
            CValidationState state;
            bool fMissingInputs;
            if (!AcceptToMemoryPool(mempool, state, tx, false, &fMissingInputs, false, false)) {
                CHECK(!state.IsInvalid(), "Transaction rejected %i: %s", state.GetRejectCode(), state.GetRejectReason());
                if (fMissingInputs) CHECK (false, "Trancsaction error: missing inputs");
                CHECK(false, state.GetRejectReason());
            }
        } else if (fHaveChain) {
            CHECK(false , "transaction already in block chain");
        }
        //CHECK(!(fInstantSend && !instantsend.ProcessTxLockRequest(tx)), "Not a valid InstantSend transaction, see debug.log for more info");
        RelayTransaction(tx);
        return true;
    } while(false);
    return false;
}




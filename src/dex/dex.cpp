
#include "dex.h"
#include "init.h"
#include "util.h"

#include "dexoffer.h"
#include "primitives/transaction.h"
#include "utilstrencodings.h"
#include "dex/dexdb.h"
#include "dextransaction.h"
#include "consensus/validation.h"
#include "chainparams.h"


#define CHECK(A,B,...) { if (!(A)) { std::string str = strprintf(std::string("%s\n") + (B), "",  ##__VA_ARGS__); LogPrintf("%s", str.c_str()); sError += str; break; } }


CDex::CDex()
{
}


CDex::CDex(const CDexOffer &dexoffer)
{
    offer = dexoffer;
}


bool CDex::CreateOffer(CDexOffer::Type type, const uint256 &idTransaction, const std::string &countryIso, const std::string &currencyIso,
           uint8_t paymentMethod, uint64_t price, uint64_t minAmount, int timeExpiration,
           const std::string &shortInfo, const std::string &details)
{
    return offer.Create(idTransaction, type, countryIso, currencyIso, paymentMethod, price, minAmount, timeExpiration, shortInfo, details);
}


bool CDex::CreateOffer(CDexOffer::Type type, const std::string &countryIso, const std::string &currencyIso,
                        uint8_t paymentMethod, uint64_t price, uint64_t minAmount, int timeExpiration,
                        const std::string &shortInfo, const std::string &details)
{
    return offer.Create(type, countryIso, currencyIso, paymentMethod, price, minAmount, timeExpiration, shortInfo, details);
}



bool CDex::addOfferToDB()
{
    if (offer.IsNull()) return false;

    dex::DexDB db(strDexDbFile);
    if (offer.isBuy())  db.addOfferBuy(offer);
    if (offer.isSell()) db.addOfferSell(offer);
    return true;
}


bool CDex::PayForOffer(uint256 &txid, std::string &sError)
{
    do {
        CHECK(!offer.IsNull(), "Offer is empty");
        CHECK(offer.idTransaction.IsNull(), "The offer has already been paid");

        CHECK(CreatePayOfferTransaction(offer, payTx, sError), "");
        offer.idTransaction = payTx.GetHash();
        txid = offer.idTransaction;
        return true;
    } while(false);
    return false;
}


bool CDex::CheckOfferTx(std::string &sError)
{
    do {
        CHECK(!offer.IsNull(), "offer is NULL");
        CHECK(offer.Check(true), "offer check fail");

        CTransaction tx;
        uint256 hashBlock;
        CHECK (GetTransaction(offer.idTransaction, tx, Params().GetConsensus(), hashBlock, true), "Transaction not found");
        CHECK(tx.vin.size() > 0, "vin empty");
        CHECK(tx.vout.size() > 0, "vout empty");
        CHECK(tx.vout[0].nValue == PAYOFFER_RETURN_FEE, "bad op_return fee");
        CHECK(tx.vout[0].scriptPubKey.IsUnspendable(), "not op_return");
        {
            uint256 hash;
            opcodetype opcode;
            std::vector<unsigned char> vch;
            CScript::const_iterator pc = tx.vout[0].scriptPubKey.begin();
            while (pc < tx.vout[0].scriptPubKey.end()) {
                CHECK(tx.vout[0].scriptPubKey.GetOp(pc, opcode, vch), "fail to getop from script");
                if (0 <= opcode && opcode <= OP_PUSHDATA4) hash.SetHex(HexStr(vch));
            }
            CHECK(offer.hash == hash, "offer hash not equal");
        }

        CAmount credit = 0;
        for (size_t i = 0; i < tx.vout.size(); i++) {
            credit += tx.vout[i].nValue;
        }

        CAmount debit = 0;
        for (auto i : tx.vin) {
            CTransaction prevtx;
            uint256 hashBlock;
            CHECK (GetTransaction(i.prevout.hash, prevtx, Params().GetConsensus(), hashBlock, true), "vin tx not found");
            CHECK (prevtx.vout.size() > i.prevout.n, "prev tx out error");
            debit += prevtx.vout[i.prevout.n].nValue;
        }
        CHECK((debit - credit) == PAYOFFER_TX_FEE, "payoffer tx fee error");
        return true;
    } while(false);
    return false;
}


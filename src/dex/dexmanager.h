
#ifndef __DEX_MANAGER_H__
#define __DEX_MANAGER_H__

#include "key.h"
#include "main.h"
#include "net.h"
#include "dex/dexdto.h"
#include "dex.h"
#include "dexoffer.h"
#include "unconfirmedoffers.h"

class CDexManager;
extern CDexManager dexman;




class CDexManager
{
public:
    CDexManager();

    void ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);

    void sendOffer(const CDexOffer &offer);

private:
    UnconfirmedOffers *uncOffers;
};




#endif // __DEX_MANAGER_H__

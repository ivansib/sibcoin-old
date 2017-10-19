
#include "rpcserver.h"

#include "clientversion.h"
#include "main.h"
#include "net.h"
#include "netbase.h"
#include "protocol.h"
#include "sync.h"
#include "timedata.h"
#include "util.h"
#include "utilstrencodings.h"
#include "version.h"
#include <boost/foreach.hpp>
#include <univalue.h>

using namespace std;


UniValue dexoffer(const UniValue& params, bool fHelp)
{

    if (fHelp)
        throw runtime_error(
            "dexoffer \n"
            "Create TEST dex offer and broadcast it.\n"
        );

    return NullUniValue;
}



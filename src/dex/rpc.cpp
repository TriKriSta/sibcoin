
#include "rpc/server.h"

#include "clientversion.h"
#include "net.h"
#include "netbase.h"
#include "protocol.h"
#include "sync.h"
#include "timedata.h"
#include "util.h"
#include "utilstrencodings.h"
#include "version.h"
#include <univalue.h>
#include "streams.h"
#include "netmessagemaker.h"

#include "dexoffer.h"
#include "random.h"
#include "dex/dexdb.h"
#include "dex.h"
#include <boost/foreach.hpp>

using namespace std;


UniValue dexoffer(const JSONRPCRequest& request)
{

    if (request.fHelp || request.params.size() > 1 || (request.params[0].get_str() != "buy" && request.params[0].get_str() != "sell"))
        throw runtime_error(
            "dexoffer buy|sell \n"
            "Create TEST dex offer and broadcast it.\n"
        );

    CDex dex;
    uint256 tr_id = GetRandHash();
    CDexOffer::Type type;
    if (request.params[0].get_str() == "buy") type = CDexOffer::BUY;
    if (request.params[0].get_str() == "sell") type = CDexOffer::SELL;

    if (!dex.CreateOffer(type, tr_id, "RU", "RUB", 1, 10000, 20000 , 10, "test shortinfo", "test details"))
        throw runtime_error("dexoffer not create\n someting wrong\n");

    auto vNodes = g_connman->CopyNodeVector(CConnman::FullyConnectedOnly);
    for (auto pNode : vNodes) {
        CNetMsgMaker msgMaker(pNode->GetSendVersion());
        g_connman->PushMessage(pNode, msgMaker.Make(NetMsgType::DEXOFFBCST, dex.offer));
    }
    g_connman->ReleaseNodeVector(vNodes);


    return NullUniValue;
}

static const CRPCCommand commands[] =
{ //  category              name                        actor (function)           okSafeMode
    //  --------------------- ------------------------    -----------------------    ----------
    { "dex",    "dexoffer",       &dexoffer,       true,  {} }
};

void RegisterDexRPCCommands(CRPCTable &t)
{
    for (unsigned int vcidx = 0; vcidx < ARRAYLEN(commands); vcidx++)
        t.appendCommand(commands[vcidx].name, &commands[vcidx]);
}
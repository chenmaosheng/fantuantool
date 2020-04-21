#ifndef _H_login_peer_RECV
#define _H_login_peer_RECV

#include "server_common.h"

struct LoginPeerRecv
{
    static void OnLoginFailedAck(PEER_CLIENT pPeerClient, uint32 iSessionId, int8 iReason);
};

#endif
#ifndef _H_login_peer_SEND
#define _H_login_peer_SEND

#include "server_common.h"

struct LoginPeerSend
{
    static int32 OnLoginFailedAck(PEER_SERVER pPeerServer, uint32 iSessionId, int8 iReason);
};

#endif
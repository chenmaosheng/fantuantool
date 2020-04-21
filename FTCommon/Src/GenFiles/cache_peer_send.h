#ifndef _H_cache_peer_SEND
#define _H_cache_peer_SEND

#include "server_common.h"

struct CachePeerSend
{
    static int32 OnLoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static int32 OnLogoutReq(PEER_SERVER pPeerServer, uint32 iSessionId);
    static int32 OnRegionEnterReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iServerId, uint16 iNameLen, const TCHAR* strAvatarName);
};

#endif
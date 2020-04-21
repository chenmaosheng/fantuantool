#ifndef _H_cache_peer_RECV
#define _H_cache_peer_RECV

#include "server_common.h"

struct CachePeerRecv
{
    static void OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static void OnLogoutReq(PEER_CLIENT pPeerClient, uint32 iSessionId);
    static void OnRegionEnterReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iServerId, uint16 iNameLen, const TCHAR* strAvatarName);
};

#endif
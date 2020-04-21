#ifndef _H_region_peer_RECV
#define _H_region_peer_RECV

#include "server_common.h"

struct RegionPeerRecv
{
    static void RegionAllocReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint64 iAvatarId, uint16 iLen, const TCHAR* strAvatarName);
    static void RegionReleaseReq(PEER_CLIENT pPeerClient, uint32 iSessionId);
    static void RegionEnterReq(PEER_CLIENT pPeerClient, uint32 iSessionId);
    static void RegionLeaveReq(PEER_CLIENT pPeerClient, uint32 iSessionId);
    static void RegionEnterAck(PEER_CLIENT pPeerClient, uint32 iSessionId, int32 iReturn);
};

#endif
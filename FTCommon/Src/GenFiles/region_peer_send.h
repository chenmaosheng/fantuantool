#ifndef _H_region_peer_SEND
#define _H_region_peer_SEND

#include "server_common.h"

struct RegionPeerSend
{
    static int32 RegionAllocReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint64 iAvatarId, uint16 iLen, const TCHAR* strAvatarName);
    static int32 RegionReleaseReq(PEER_SERVER pPeerServer, uint32 iSessionId);
    static int32 RegionEnterReq(PEER_SERVER pPeerServer, uint32 iSessionId);
    static int32 RegionLeaveReq(PEER_SERVER pPeerServer, uint32 iSessionId);
    static int32 RegionEnterAck(PEER_SERVER pPeerServer, uint32 iSessionId, int32 iReturn);
};

#endif
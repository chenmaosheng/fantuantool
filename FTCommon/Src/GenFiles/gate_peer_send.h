#ifndef _H_gate_peer_SEND
#define _H_gate_peer_SEND

#include "server_common.h"

struct GatePeerSend
{
    static int32 GateAllocReq(PEER_SERVER pPeerServer, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static int32 GateReleaseReq(PEER_SERVER pPeerServer, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static int32 RegionBindReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iRegionServerId);
    static int32 BroadcastData(PEER_SERVER pPeerServer, uint16 iSessionCount, const uint32* arraySessionId, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif
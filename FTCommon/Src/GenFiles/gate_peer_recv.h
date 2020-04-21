#ifndef _H_gate_peer_RECV
#define _H_gate_peer_RECV

#include "server_common.h"

struct GatePeerRecv
{
    static void GateAllocReq(PEER_CLIENT pPeerClient, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static void GateReleaseReq(PEER_CLIENT pPeerClient, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static void RegionBindReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iRegionServerId);
    static void BroadcastData(PEER_CLIENT pPeerClient, uint16 iSessionCount, const uint32* arraySessionId, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif
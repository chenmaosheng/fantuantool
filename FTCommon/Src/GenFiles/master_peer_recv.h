#ifndef _H_master_peer_RECV
#define _H_master_peer_RECV

#include "server_common.h"

struct MasterPeerRecv
{
    static void LoginReportState(PEER_CLIENT pPeerClient, uint8 iServerId);
    static void GateReportState(PEER_CLIENT pPeerClient, uint8 iServerId, uint16 iSessionCount);
    static void RegionReportState(PEER_CLIENT pPeerClient, uint8 iServerId, uint16 iPlayerCount);
    static void CacheReportState(PEER_CLIENT pPeerClient, uint8 iServerId);
    static void OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static void GateAllocAck(PEER_CLIENT pPeerClient, uint8 iServerId, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName, uint32 iGateSessionId);
    static void OnGateLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static void OnRegionAllocAck(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iServerId, int32 iReturn);
    static void OnRegionLeaveReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint8 iServerId);
    static void OnRegionPlayerFailReq(PEER_CLIENT pPeerClient, uint32 iSessionId, int32 iReason);
};

#endif
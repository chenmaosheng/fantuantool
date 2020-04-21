#ifndef _H_master_peer_SEND
#define _H_master_peer_SEND

#include "server_common.h"

struct MasterPeerSend
{
    static int32 LoginReportState(PEER_SERVER pPeerServer, uint8 iServerId);
    static int32 GateReportState(PEER_SERVER pPeerServer, uint8 iServerId, uint16 iSessionCount);
    static int32 RegionReportState(PEER_SERVER pPeerServer, uint8 iServerId, uint16 iPlayerCount);
    static int32 CacheReportState(PEER_SERVER pPeerServer, uint8 iServerId);
    static int32 OnLoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static int32 GateAllocAck(PEER_SERVER pPeerServer, uint8 iServerId, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName, uint32 iGateSessionId);
    static int32 OnGateLoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName);
    static int32 OnRegionAllocAck(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iServerId, int32 iReturn);
    static int32 OnRegionLeaveReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iServerId);
    static int32 OnRegionPlayerFailReq(PEER_SERVER pPeerServer, uint32 iSessionId, int32 iReason);
};

#endif
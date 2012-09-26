#ifndef _H_MASTER_PEER_RECV
#define _H_MASTER_PEER_RECV

#include "server_common.h"

struct MasterPeerRecv
{
	static void OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, const char* strAccountName);
	static void GateHoldAck(PEER_CLIENT pPeerClient, uint16 iServerId, uint32 iLoginSessionId, const TCHAR* strAccountName, uint32 iGateSessionId);
};

#endif

#ifndef _H_MASTER_PEER_SEND
#define _H_MASTER_PEER_SEND

#include "server_common.h"

struct MasterPeerSend
{
	static uint32 LoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, const char* strAccountName);
	static uint32 GateHoldAck(PEER_SERVER pPeerServer, uint16 iServerId, uint32 iLoginSessionId, const TCHAR* strAccountName, uint32 iGateSessionId);
};

#endif

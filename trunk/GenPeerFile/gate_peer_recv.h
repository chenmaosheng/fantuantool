#ifndef _H_GATE_PEER_RECV
#define _H_GATE_PEER_RECV

#include "server_common.h"

struct GatePeerRecv
{
	static void GateHoldReq(PEER_CLIENT pPeerClient, uint32 iSessionId, const TCHAR* strAccountName);
};

#endif

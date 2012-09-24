#ifndef _H_MASTER_PEER_SEND
#define _H_MASTER_PEER_SEND

#include "peer_packet.h"

struct MasterPeerSend
{
	static uint32 LoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, const char* strAccountName);
};

#endif

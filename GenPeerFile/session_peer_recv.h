#ifndef _H_SESSION_PEER_RECV
#define _H_SESSION_PEER_RECV

#include "server_common.h"

struct SessionPeerRecv
{
	static void PacketForward(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iType, uint16 iLen, const char *pBuf);
};

#endif

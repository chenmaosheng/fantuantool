#ifndef _H_SESSION_PEER_SEND
#define _H_SESSION_PEER_SEND

#include "server_common.h"

struct SessionPeerSend
{
	static int32 PacketForward(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif

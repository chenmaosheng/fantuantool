#ifndef _H_session_peer_SEND
#define _H_session_peer_SEND

#include "server_common.h"

struct SessionPeerSend
{
    static int32 PacketForward(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf);
    static int32 OnSessionDisconnect(PEER_SERVER pPeerServer, uint32 iSessionId);
    static int32 Disconnect(PEER_SERVER pPeerServer, uint32 iSessionid, uint8 iReason);
    static int32 SendData(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif
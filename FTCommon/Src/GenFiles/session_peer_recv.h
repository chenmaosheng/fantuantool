#ifndef _H_session_peer_RECV
#define _H_session_peer_RECV

#include "server_common.h"

struct SessionPeerRecv
{
    static void PacketForward(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf);
    static void OnSessionDisconnect(PEER_CLIENT pPeerClient, uint32 iSessionId);
    static void Disconnect(PEER_CLIENT pPeerClient, uint32 iSessionid, uint8 iReason);
    static void SendData(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif
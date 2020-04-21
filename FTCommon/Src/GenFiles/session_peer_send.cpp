#include "session_peer_send.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

int32 SessionPeerSend::PacketForward(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
    PeerOutputStream stream;
    uint16 pBuf_length;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iTypeId)) return -1;
    if (!stream.Serialize(iLen)) return -1;
    pBuf_length = iLen;
    if (!stream.Serialize(pBuf_length)) return -1;
    if (!stream.Serialize(pBuf_length, pBuf)) return -1;
    stream.SetId(PEER_FILTER_SESSION, 0);
    return stream.Send(pPeerServer);
}

int32 SessionPeerSend::OnSessionDisconnect(PEER_SERVER pPeerServer, uint32 iSessionId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    stream.SetId(PEER_FILTER_SESSION, 1);
    return stream.Send(pPeerServer);
}

int32 SessionPeerSend::Disconnect(PEER_SERVER pPeerServer, uint32 iSessionid, uint8 iReason)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionid)) return -1;
    if (!stream.Serialize(iReason)) return -1;
    stream.SetId(PEER_FILTER_SESSION, 2);
    return stream.Send(pPeerServer);
}

int32 SessionPeerSend::SendData(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
    PeerOutputStream stream;
    uint16 pBuf_length;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iTypeId)) return -1;
    if (!stream.Serialize(iLen)) return -1;
    pBuf_length = iLen;
    if (!stream.Serialize(pBuf_length)) return -1;
    if (!stream.Serialize(pBuf_length, pBuf)) return -1;
    stream.SetId(PEER_FILTER_SESSION, 3);
    return stream.Send(pPeerServer);
}


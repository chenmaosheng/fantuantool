#include "session_peer_recv.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

namespace pr_SessionPeer_recv
{

bool CALLBACK PacketForward_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint16 iTypeId;
    uint16 iLen;
    char* pBuf;
    uint16 pBuf_length;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iTypeId)) return false;
    if (!stream.Serialize(iLen)) return false;
    pBuf_length = iLen;
    if (!stream.Serialize(pBuf_length)) return false;
    if (!stream.Serialize(pBuf_length, pBuf)) return false;
    SessionPeerRecv::PacketForward(pPeerClient, iSessionId, iTypeId, iLen, pBuf);
    return true;
}

bool CALLBACK OnSessionDisconnect_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    if (!stream.Serialize(iSessionId)) return false;
    SessionPeerRecv::OnSessionDisconnect(pPeerClient, iSessionId);
    return true;
}

bool CALLBACK Disconnect_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionid;
    uint8 iReason;
    if (!stream.Serialize(iSessionid)) return false;
    if (!stream.Serialize(iReason)) return false;
    SessionPeerRecv::Disconnect(pPeerClient, iSessionid, iReason);
    return true;
}

bool CALLBACK SendData_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint16 iTypeId;
    uint16 iLen;
    char* pBuf;
    uint16 pBuf_length;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iTypeId)) return false;
    if (!stream.Serialize(iLen)) return false;
    pBuf_length = iLen;
    if (!stream.Serialize(pBuf_length)) return false;
    if (!stream.Serialize(pBuf_length, pBuf)) return false;
    SessionPeerRecv::SendData(pPeerClient, iSessionId, iTypeId, iLen, pBuf);
    return true;
}

static PeerClientDispatchFilter::Func func[] = 
{
    PacketForward_Callback,
    OnSessionDisconnect_Callback,
    Disconnect_Callback,
    SendData_Callback,
    NULL
};

struct SessionPeerDispatch
{
    SessionPeerDispatch()
    {
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_SESSION).m_pFunc = func;
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_SESSION).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static pr_SessionPeer_recv::SessionPeerDispatch _SessionPeerDispatch;


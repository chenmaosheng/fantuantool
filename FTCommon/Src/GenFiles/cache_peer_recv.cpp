#include "cache_peer_recv.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

namespace pr_CachePeer_recv
{

bool CALLBACK OnLoginReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint16 iAccountNameLen;
    TCHAR* strAccountName;
    uint16 strAccountName_length;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iAccountNameLen)) return false;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return false;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return false;
    CachePeerRecv::OnLoginReq(pPeerClient, iSessionId, iAccountNameLen, strAccountName);
    return true;
}

bool CALLBACK OnLogoutReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    if (!stream.Serialize(iSessionId)) return false;
    CachePeerRecv::OnLogoutReq(pPeerClient, iSessionId);
    return true;
}

bool CALLBACK OnRegionEnterReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint8 iServerId;
    uint16 iNameLen;
    TCHAR* strAvatarName;
    uint16 strAvatarName_length;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iServerId)) return false;
    if (!stream.Serialize(iNameLen)) return false;
    strAvatarName_length = iNameLen;
    if (!stream.Serialize(strAvatarName_length)) return false;
    if (!stream.Serialize(strAvatarName_length, strAvatarName)) return false;
    CachePeerRecv::OnRegionEnterReq(pPeerClient, iSessionId, iServerId, iNameLen, strAvatarName);
    return true;
}

static PeerClientDispatchFilter::Func func[] = 
{
    OnLoginReq_Callback,
    OnLogoutReq_Callback,
    OnRegionEnterReq_Callback,
    NULL
};

struct CachePeerDispatch
{
    CachePeerDispatch()
    {
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_CACHE).m_pFunc = func;
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_CACHE).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static pr_CachePeer_recv::CachePeerDispatch _CachePeerDispatch;


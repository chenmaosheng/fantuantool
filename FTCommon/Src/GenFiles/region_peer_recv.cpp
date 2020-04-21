#include "region_peer_recv.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

namespace pr_RegionPeer_recv
{

bool CALLBACK RegionAllocReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint64 iAvatarId;
    uint16 iLen;
    TCHAR* strAvatarName;
    uint16 strAvatarName_length;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iAvatarId)) return false;
    if (!stream.Serialize(iLen)) return false;
    strAvatarName_length = iLen;
    if (!stream.Serialize(strAvatarName_length)) return false;
    if (!stream.Serialize(strAvatarName_length, strAvatarName)) return false;
    RegionPeerRecv::RegionAllocReq(pPeerClient, iSessionId, iAvatarId, iLen, strAvatarName);
    return true;
}

bool CALLBACK RegionReleaseReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    if (!stream.Serialize(iSessionId)) return false;
    RegionPeerRecv::RegionReleaseReq(pPeerClient, iSessionId);
    return true;
}

bool CALLBACK RegionEnterReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    if (!stream.Serialize(iSessionId)) return false;
    RegionPeerRecv::RegionEnterReq(pPeerClient, iSessionId);
    return true;
}

bool CALLBACK RegionLeaveReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    if (!stream.Serialize(iSessionId)) return false;
    RegionPeerRecv::RegionLeaveReq(pPeerClient, iSessionId);
    return true;
}

bool CALLBACK RegionEnterAck_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    int32 iReturn;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iReturn)) return false;
    RegionPeerRecv::RegionEnterAck(pPeerClient, iSessionId, iReturn);
    return true;
}

static PeerClientDispatchFilter::Func func[] = 
{
    RegionAllocReq_Callback,
    RegionReleaseReq_Callback,
    RegionEnterReq_Callback,
    RegionLeaveReq_Callback,
    RegionEnterAck_Callback,
    NULL
};

struct RegionPeerDispatch
{
    RegionPeerDispatch()
    {
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_REGION).m_pFunc = func;
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_REGION).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static pr_RegionPeer_recv::RegionPeerDispatch _RegionPeerDispatch;


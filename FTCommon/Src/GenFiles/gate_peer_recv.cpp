#include "gate_peer_recv.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

namespace pr_GatePeer_recv
{

bool CALLBACK GateAllocReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iLoginSessionId;
    uint16 iAccountNameLen;
    TCHAR* strAccountName;
    uint16 strAccountName_length;
    if (!stream.Serialize(iLoginSessionId)) return false;
    if (!stream.Serialize(iAccountNameLen)) return false;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return false;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return false;
    GatePeerRecv::GateAllocReq(pPeerClient, iLoginSessionId, iAccountNameLen, strAccountName);
    return true;
}

bool CALLBACK GateReleaseReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iLoginSessionId;
    uint16 iAccountNameLen;
    TCHAR* strAccountName;
    uint16 strAccountName_length;
    if (!stream.Serialize(iLoginSessionId)) return false;
    if (!stream.Serialize(iAccountNameLen)) return false;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return false;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return false;
    GatePeerRecv::GateReleaseReq(pPeerClient, iLoginSessionId, iAccountNameLen, strAccountName);
    return true;
}

bool CALLBACK RegionBindReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint8 iRegionServerId;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iRegionServerId)) return false;
    GatePeerRecv::RegionBindReq(pPeerClient, iSessionId, iRegionServerId);
    return true;
}

bool CALLBACK BroadcastData_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint16 iSessionCount;
    uint32* arraySessionId;
    uint16 iTypeId;
    uint16 iLen;
    char* pBuf;
    uint16 arraySessionId_length;
    uint16 pBuf_length;
    if (!stream.Serialize(iSessionCount)) return false;
    arraySessionId_length = iSessionCount;
    if (!stream.Serialize(arraySessionId_length)) return false;
    if (!stream.Serialize(arraySessionId_length, arraySessionId)) return false;
    if (!stream.Serialize(iTypeId)) return false;
    if (!stream.Serialize(iLen)) return false;
    pBuf_length = iLen;
    if (!stream.Serialize(pBuf_length)) return false;
    if (!stream.Serialize(pBuf_length, pBuf)) return false;
    GatePeerRecv::BroadcastData(pPeerClient, iSessionCount, arraySessionId, iTypeId, iLen, pBuf);
    return true;
}

static PeerClientDispatchFilter::Func func[] = 
{
    GateAllocReq_Callback,
    GateReleaseReq_Callback,
    RegionBindReq_Callback,
    BroadcastData_Callback,
    NULL
};

struct GatePeerDispatch
{
    GatePeerDispatch()
    {
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_GATE).m_pFunc = func;
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_GATE).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static pr_GatePeer_recv::GatePeerDispatch _GatePeerDispatch;


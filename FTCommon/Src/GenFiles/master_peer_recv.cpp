#include "master_peer_recv.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

namespace pr_MasterPeer_recv
{

bool CALLBACK LoginReportState_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint8 iServerId;
    if (!stream.Serialize(iServerId)) return false;
    MasterPeerRecv::LoginReportState(pPeerClient, iServerId);
    return true;
}

bool CALLBACK GateReportState_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint8 iServerId;
    uint16 iSessionCount;
    if (!stream.Serialize(iServerId)) return false;
    if (!stream.Serialize(iSessionCount)) return false;
    MasterPeerRecv::GateReportState(pPeerClient, iServerId, iSessionCount);
    return true;
}

bool CALLBACK RegionReportState_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint8 iServerId;
    uint16 iPlayerCount;
    if (!stream.Serialize(iServerId)) return false;
    if (!stream.Serialize(iPlayerCount)) return false;
    MasterPeerRecv::RegionReportState(pPeerClient, iServerId, iPlayerCount);
    return true;
}

bool CALLBACK CacheReportState_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint8 iServerId;
    if (!stream.Serialize(iServerId)) return false;
    MasterPeerRecv::CacheReportState(pPeerClient, iServerId);
    return true;
}

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
    MasterPeerRecv::OnLoginReq(pPeerClient, iSessionId, iAccountNameLen, strAccountName);
    return true;
}

bool CALLBACK GateAllocAck_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint8 iServerId;
    uint32 iLoginSessionId;
    uint16 iAccountNameLen;
    TCHAR* strAccountName;
    uint32 iGateSessionId;
    uint16 strAccountName_length;
    if (!stream.Serialize(iServerId)) return false;
    if (!stream.Serialize(iLoginSessionId)) return false;
    if (!stream.Serialize(iAccountNameLen)) return false;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return false;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return false;
    if (!stream.Serialize(iGateSessionId)) return false;
    MasterPeerRecv::GateAllocAck(pPeerClient, iServerId, iLoginSessionId, iAccountNameLen, strAccountName, iGateSessionId);
    return true;
}

bool CALLBACK OnGateLoginReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
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
    MasterPeerRecv::OnGateLoginReq(pPeerClient, iSessionId, iAccountNameLen, strAccountName);
    return true;
}

bool CALLBACK OnRegionAllocAck_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint8 iServerId;
    int32 iReturn;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iServerId)) return false;
    if (!stream.Serialize(iReturn)) return false;
    MasterPeerRecv::OnRegionAllocAck(pPeerClient, iSessionId, iServerId, iReturn);
    return true;
}

bool CALLBACK OnRegionLeaveReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    uint8 iServerId;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iServerId)) return false;
    MasterPeerRecv::OnRegionLeaveReq(pPeerClient, iSessionId, iServerId);
    return true;
}

bool CALLBACK OnRegionPlayerFailReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
    uint32 iSessionId;
    int32 iReason;
    if (!stream.Serialize(iSessionId)) return false;
    if (!stream.Serialize(iReason)) return false;
    MasterPeerRecv::OnRegionPlayerFailReq(pPeerClient, iSessionId, iReason);
    return true;
}

static PeerClientDispatchFilter::Func func[] = 
{
    LoginReportState_Callback,
    GateReportState_Callback,
    RegionReportState_Callback,
    CacheReportState_Callback,
    OnLoginReq_Callback,
    GateAllocAck_Callback,
    OnGateLoginReq_Callback,
    OnRegionAllocAck_Callback,
    OnRegionLeaveReq_Callback,
    OnRegionPlayerFailReq_Callback,
    NULL
};

struct MasterPeerDispatch
{
    MasterPeerDispatch()
    {
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_MASTER).m_pFunc = func;
        PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_MASTER).m_iFuncCount = sizeof(func)/sizeof(func[0]);
    }
};

}

static pr_MasterPeer_recv::MasterPeerDispatch _MasterPeerDispatch;


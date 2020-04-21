#include "master_peer_send.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

int32 MasterPeerSend::LoginReportState(PEER_SERVER pPeerServer, uint8 iServerId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iServerId)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 0);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::GateReportState(PEER_SERVER pPeerServer, uint8 iServerId, uint16 iSessionCount)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iServerId)) return -1;
    if (!stream.Serialize(iSessionCount)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 1);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::RegionReportState(PEER_SERVER pPeerServer, uint8 iServerId, uint16 iPlayerCount)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iServerId)) return -1;
    if (!stream.Serialize(iPlayerCount)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 2);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::CacheReportState(PEER_SERVER pPeerServer, uint8 iServerId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iServerId)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 3);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::OnLoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
    PeerOutputStream stream;
    uint16 strAccountName_length;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iAccountNameLen)) return -1;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return -1;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 4);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::GateAllocAck(PEER_SERVER pPeerServer, uint8 iServerId, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName, uint32 iGateSessionId)
{
    PeerOutputStream stream;
    uint16 strAccountName_length;
    if (!stream.Serialize(iServerId)) return -1;
    if (!stream.Serialize(iLoginSessionId)) return -1;
    if (!stream.Serialize(iAccountNameLen)) return -1;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return -1;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return -1;
    if (!stream.Serialize(iGateSessionId)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 5);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::OnGateLoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
    PeerOutputStream stream;
    uint16 strAccountName_length;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iAccountNameLen)) return -1;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return -1;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 6);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::OnRegionAllocAck(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iServerId, int32 iReturn)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iServerId)) return -1;
    if (!stream.Serialize(iReturn)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 7);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::OnRegionLeaveReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iServerId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iServerId)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 8);
    return stream.Send(pPeerServer);
}

int32 MasterPeerSend::OnRegionPlayerFailReq(PEER_SERVER pPeerServer, uint32 iSessionId, int32 iReason)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iReason)) return -1;
    stream.SetId(PEER_FILTER_MASTER, 9);
    return stream.Send(pPeerServer);
}


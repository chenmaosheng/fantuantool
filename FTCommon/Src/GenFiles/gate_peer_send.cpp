#include "gate_peer_send.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

int32 GatePeerSend::GateAllocReq(PEER_SERVER pPeerServer, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
    PeerOutputStream stream;
    uint16 strAccountName_length;
    if (!stream.Serialize(iLoginSessionId)) return -1;
    if (!stream.Serialize(iAccountNameLen)) return -1;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return -1;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return -1;
    stream.SetId(PEER_FILTER_GATE, 0);
    return stream.Send(pPeerServer);
}

int32 GatePeerSend::GateReleaseReq(PEER_SERVER pPeerServer, uint32 iLoginSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
    PeerOutputStream stream;
    uint16 strAccountName_length;
    if (!stream.Serialize(iLoginSessionId)) return -1;
    if (!stream.Serialize(iAccountNameLen)) return -1;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return -1;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return -1;
    stream.SetId(PEER_FILTER_GATE, 1);
    return stream.Send(pPeerServer);
}

int32 GatePeerSend::RegionBindReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iRegionServerId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iRegionServerId)) return -1;
    stream.SetId(PEER_FILTER_GATE, 2);
    return stream.Send(pPeerServer);
}

int32 GatePeerSend::BroadcastData(PEER_SERVER pPeerServer, uint16 iSessionCount, const uint32* arraySessionId, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
    PeerOutputStream stream;
    uint16 arraySessionId_length;
    uint16 pBuf_length;
    if (!stream.Serialize(iSessionCount)) return -1;
    arraySessionId_length = iSessionCount;
    if (!stream.Serialize(arraySessionId_length)) return -1;
    if (!stream.Serialize(arraySessionId_length, arraySessionId)) return -1;
    if (!stream.Serialize(iTypeId)) return -1;
    if (!stream.Serialize(iLen)) return -1;
    pBuf_length = iLen;
    if (!stream.Serialize(pBuf_length)) return -1;
    if (!stream.Serialize(pBuf_length, pBuf)) return -1;
    stream.SetId(PEER_FILTER_GATE, 3);
    return stream.Send(pPeerServer);
}


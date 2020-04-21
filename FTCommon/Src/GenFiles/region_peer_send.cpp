#include "region_peer_send.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

int32 RegionPeerSend::RegionAllocReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint64 iAvatarId, uint16 iLen, const TCHAR* strAvatarName)
{
    PeerOutputStream stream;
    uint16 strAvatarName_length;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iAvatarId)) return -1;
    if (!stream.Serialize(iLen)) return -1;
    strAvatarName_length = iLen;
    if (!stream.Serialize(strAvatarName_length)) return -1;
    if (!stream.Serialize(strAvatarName_length, strAvatarName)) return -1;
    stream.SetId(PEER_FILTER_REGION, 0);
    return stream.Send(pPeerServer);
}

int32 RegionPeerSend::RegionReleaseReq(PEER_SERVER pPeerServer, uint32 iSessionId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    stream.SetId(PEER_FILTER_REGION, 1);
    return stream.Send(pPeerServer);
}

int32 RegionPeerSend::RegionEnterReq(PEER_SERVER pPeerServer, uint32 iSessionId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    stream.SetId(PEER_FILTER_REGION, 2);
    return stream.Send(pPeerServer);
}

int32 RegionPeerSend::RegionLeaveReq(PEER_SERVER pPeerServer, uint32 iSessionId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    stream.SetId(PEER_FILTER_REGION, 3);
    return stream.Send(pPeerServer);
}

int32 RegionPeerSend::RegionEnterAck(PEER_SERVER pPeerServer, uint32 iSessionId, int32 iReturn)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iReturn)) return -1;
    stream.SetId(PEER_FILTER_REGION, 4);
    return stream.Send(pPeerServer);
}


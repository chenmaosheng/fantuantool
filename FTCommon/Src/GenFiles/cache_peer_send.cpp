#include "cache_peer_send.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

int32 CachePeerSend::OnLoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR* strAccountName)
{
    PeerOutputStream stream;
    uint16 strAccountName_length;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iAccountNameLen)) return -1;
    strAccountName_length = iAccountNameLen;
    if (!stream.Serialize(strAccountName_length)) return -1;
    if (!stream.Serialize(strAccountName_length, strAccountName)) return -1;
    stream.SetId(PEER_FILTER_CACHE, 0);
    return stream.Send(pPeerServer);
}

int32 CachePeerSend::OnLogoutReq(PEER_SERVER pPeerServer, uint32 iSessionId)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    stream.SetId(PEER_FILTER_CACHE, 1);
    return stream.Send(pPeerServer);
}

int32 CachePeerSend::OnRegionEnterReq(PEER_SERVER pPeerServer, uint32 iSessionId, uint8 iServerId, uint16 iNameLen, const TCHAR* strAvatarName)
{
    PeerOutputStream stream;
    uint16 strAvatarName_length;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iServerId)) return -1;
    if (!stream.Serialize(iNameLen)) return -1;
    strAvatarName_length = iNameLen;
    if (!stream.Serialize(strAvatarName_length)) return -1;
    if (!stream.Serialize(strAvatarName_length, strAvatarName)) return -1;
    stream.SetId(PEER_FILTER_CACHE, 2);
    return stream.Send(pPeerServer);
}


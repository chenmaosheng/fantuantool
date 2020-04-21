#include "login_peer_send.h"
#include "peer_dispatch.h"
#include "peer_stream.h"
#include <malloc.h>

int32 LoginPeerSend::OnLoginFailedAck(PEER_SERVER pPeerServer, uint32 iSessionId, int8 iReason)
{
    PeerOutputStream stream;
    if (!stream.Serialize(iSessionId)) return -1;
    if (!stream.Serialize(iReason)) return -1;
    stream.SetId(PEER_FILTER_LOGIN, 0);
    return stream.Send(pPeerServer);
}


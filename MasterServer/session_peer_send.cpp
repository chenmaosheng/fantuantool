#include "session_peer_send.h"
#include "peer_stream.h"

int32 SessionPeerSend::PacketForward(PEER_SERVER pPeerServer, uint32 iSessionId, uint16 iType, uint16 iLen, const char *pBuf)
{
	PeerOutputStream stream;
	stream.Serialize(iSessionId);
	stream.Serialize(iType);
	stream.Serialize(iLen);
	stream.Serialize(iLen, pBuf);

	stream.SetId(PEER_FILTER_SESSION, 0);
	return stream.Send(pPeerServer);
}
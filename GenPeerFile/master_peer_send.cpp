#include "master_peer_send.h"
#include "peer_stream.h"

uint32 MasterPeerSend::LoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, const char *strAccountName)
{
	PeerOutputStream stream;
	uint16 iAccountNameLen = (uint16)strlen(strAccountName);
	stream.Serialize(iSessionId);
	stream.Serialize(iAccountNameLen);
	stream.Serialize(iAccountNameLen, strAccountName);

	stream.SetId(1, 0);
	return stream.Send(pPeerServer);
}

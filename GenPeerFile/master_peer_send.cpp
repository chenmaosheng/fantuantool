#include "master_peer_send.h"
#include "peer_stream.h"

uint32 MasterPeerSend::LoginReq(PEER_SERVER pPeerServer, uint32 iSessionId, const TCHAR *strAccountName)
{
	PeerOutputStream stream;
	uint16 iAccountNameLen = (uint16)wcslen(strAccountName);
	stream.Serialize(iSessionId);
	stream.Serialize(iAccountNameLen);
	stream.Serialize(iAccountNameLen, strAccountName);

	stream.SetId(PEER_FILTER_MASTER, 0);
	return stream.Send(pPeerServer);
}

uint32 MasterPeerSend::GateHoldAck(PEER_SERVER pPeerServer, uint16 iServerId, uint32 iLoginSessionId, const TCHAR* strAccountName, uint32 iGateSessionId)
{
	PeerOutputStream stream;
	uint16 iAccountNameLen = (uint16)wcslen(strAccountName);
	stream.Serialize(iServerId);
	stream.Serialize(iLoginSessionId);
	stream.Serialize(iAccountNameLen);
	stream.Serialize(iAccountNameLen, strAccountName);
	stream.Serialize(iGateSessionId);

	stream.SetId(PEER_FILTER_MASTER, 1);
	return stream.Send(pPeerServer);
}
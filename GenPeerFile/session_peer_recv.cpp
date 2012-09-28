#include "session_peer_recv.h"
#include "peer_stream.h"
#include "session_peer_dispatch.h"
#include "peer_dispatch.h"
#include <malloc.h>

bool CALLBACK PacketForward_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
	uint32 iSessionId = 0;
	stream.Serialize(iSessionId);
	uint16 iTypeId = 0;
	stream.Serialize(iTypeId);
	uint16 iLen = 0;
	stream.Serialize(iLen);
	char* pBuf = (char*)_malloca(iLen+1);
	stream.Serialize(iLen, pBuf);
	pBuf[iLen] = '\0';

	SessionPeerRecv::PacketForward(pPeerClient, iSessionId, iTypeId, iLen, pBuf);

	return true;
}

static PeerClientDispatchFilter::Func func[] = 
{
	PacketForward_Callback,
	NULL
};

SessionPeerDispatch::SessionPeerDispatch()
{
	PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_SESSION).m_pFunc = func;
	PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_SESSION).m_iFuncCount = sizeof(func)/sizeof(func[0]);
}

static SessionPeerDispatch _MasterPeerDispatch;
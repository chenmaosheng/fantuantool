#include "gate_peer_recv.h"
#include "peer_stream.h"
#include "gate_peer_dispatch.h"
#include "peer_dispatch.h"
#include <malloc.h>

bool CALLBACK GateHoldReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
	uint32 iSessionId = 0;
	stream.Serialize(iSessionId);
	uint16 iAccountNameLen = 0;
	stream.Serialize(iAccountNameLen);
	TCHAR* accountName = (TCHAR*)_malloca((iAccountNameLen+1)*sizeof(TCHAR));
	stream.Serialize(iAccountNameLen, accountName);
	accountName[iAccountNameLen] = _T('\0');

	GatePeerRecv::GateHoldReq(pPeerClient, iSessionId, accountName);

	return true;
}

static PeerClientDispatchFilter::Func test_func[] = 
{
	GateHoldReq_Callback,
	NULL
};

GatePeerDispatch::GatePeerDispatch()
{
	PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_GATE).m_pFunc = test_func;
}

static GatePeerDispatch _GatePeerDispatch;
#include "master_peer_recv.h"
#include "peer_stream.h"
#include "master_peer_dispatch.h"
#include "peer_dispatch.h"
#include <malloc.h>

bool CALLBACK OnLoginReq_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
	uint32 iSessionId = 0;
	stream.Serialize(iSessionId);
	uint16 iAccountNameLen = 0;
	stream.Serialize(iAccountNameLen);
	char* accountName = (char*)_malloca(iAccountNameLen+1);
	stream.Serialize(iAccountNameLen, accountName);
	accountName[iAccountNameLen] = '\0';

	MasterPeerRecv::OnLoginReq(pPeerClient, iSessionId, accountName);

	return true;
}

bool CALLBACK GateHoldAck_Callback(PEER_CLIENT pPeerClient, PeerInputStream& stream)
{
	uint16 iServerId = 0;
	stream.Serialize(iServerId);
	uint32 iLoginSessionId = 0;
	stream.Serialize(iLoginSessionId);
	uint16 iAccountNameLen = 0;
	stream.Serialize(iAccountNameLen);
	TCHAR* accountName = (TCHAR*)_malloca((iAccountNameLen+1)*sizeof(TCHAR));
	stream.Serialize(iAccountNameLen, accountName);
	accountName[iAccountNameLen] = _T('\0');
	uint32 iGateSessionId = 0;
	stream.Serialize(iGateSessionId);

	MasterPeerRecv::GateHoldAck(pPeerClient, iServerId, iLoginSessionId, accountName, iGateSessionId);

	return true;
}

static PeerClientDispatchFilter::Func test_func[] = 
{
	OnLoginReq_Callback,
	GateHoldAck_Callback,
	NULL
};

MasterPeerDispatch::MasterPeerDispatch()
{
	PeerClientDispatchFilterArray::GetFilter(PEER_FILTER_MASTER).m_pFunc = test_func;
}

static MasterPeerDispatch _MasterPeerDispatch;
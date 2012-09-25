#include "peer_dispatch.h"
#include "peer_client.h"
#include "peer_server.h"

PeerClientDispatchFilter PeerClientDispatchFilterArray::m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX] = {0};
PeerServerDispatchFilter PeerServerDispatchFilterArray::m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX] = {0};

PeerClientDispatchFilter& PeerClientDispatchFilterArray::GetFilter(uint16 iFilterId)
{
	return m_arrDispatchFilter[iFilterId];
}

bool PeerServer::Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf)
{
	PeerInputStream stream(iLen, pBuf);	// step4: put buffer into stream
	return PeerServerDispatchFilterArray::GetFilter(iFilterId).m_pFunc[iFuncId]((PEER_SERVER)this, stream);
}

PeerServerDispatchFilter& PeerServerDispatchFilterArray::GetFilter(uint16 iFilterId)
{
	return m_arrDispatchFilter[iFilterId];
}

bool PeerClient::Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf)
{
	PeerInputStream stream(iLen, pBuf);	// step4: put buffer into stream
	return PeerClientDispatchFilterArray::GetFilter(iFilterId).m_pFunc[iFuncId]((PEER_CLIENT)this, stream);
}
#include "peer_dispatch.h"
#include "peer_client_connector.h"
#include "peer_server_connector.h"

PeerClientDispatchFilter PeerClientDispatchFilterArray::m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX] = {0};
PeerServerDispatchFilter PeerServerDispatchFilterArray::m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX] = {0};

PeerClientDispatchFilter& PeerClientDispatchFilterArray::GetFilter(uint16 iFilterId)
{
	return m_arrDispatchFilter[iFilterId];
}

bool PeerClientConnector::Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf)
{
	PeerInputStream stream(iLen, pBuf);	// step4: put buffer into stream
	return PeerClientDispatchFilterArray::GetFilter(iFilterId).m_pFunc[iFuncId]((PEER_CLIENT_CONNECTOR)this, stream);
}

PeerServerDispatchFilter& PeerServerDispatchFilterArray::GetFilter(uint16 iFilterId)
{
	return m_arrDispatchFilter[iFilterId];
}

bool PeerServerConnector::Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf)
{
	PeerInputStream stream(iLen, pBuf);	// step4: put buffer into stream
	return PeerServerDispatchFilterArray::GetFilter(iFilterId).m_pFunc[iFuncId]((PEER_SERVER_CONNECTOR)this, stream);
}
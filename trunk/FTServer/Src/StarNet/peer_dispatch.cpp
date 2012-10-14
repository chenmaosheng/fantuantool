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
	PeerServerDispatchFilter& filter = PeerServerDispatchFilterArray::GetFilter(iFilterId);
	// check if func type is valid
	_ASSERT(iFuncId < filter.m_iFuncCount);
	SN_LOG_DBG(_T("FilterId=%d FuncId=%d Len=%d"), iFilterId, iFuncId, iLen);
	if (iFuncId >= filter.m_iFuncCount)
	{
		SN_LOG_ERR(_T("invalid funcId=%d, funcCount=%d"), iFuncId, filter.m_iFuncCount);
		return false;
	}

	PeerInputStream stream(iLen, pBuf);	// put buffer into stream
	return filter.m_pFunc[iFuncId]((PEER_SERVER)this, stream);
}

PeerServerDispatchFilter& PeerServerDispatchFilterArray::GetFilter(uint16 iFilterId)
{
	return m_arrDispatchFilter[iFilterId];
}

bool PeerClient::Dispatch(uint16 iFilterId, uint16 iFuncId, uint32 iLen, char* pBuf)
{
	PeerClientDispatchFilter& filter = PeerClientDispatchFilterArray::GetFilter(iFilterId);
	// check if func type is valid
	_ASSERT(iFuncId < filter.m_iFuncCount);
	SN_LOG_DBG(_T("FilterId=%d FuncId=%d Len=%d"), iFilterId, iFuncId, iLen);
	if (iFuncId >= filter.m_iFuncCount)
	{
		SN_LOG_ERR(_T("invalid funcId=%d, funcCount=%d"), iFuncId, filter.m_iFuncCount);
		return false;
	}
	PeerInputStream stream(iLen, pBuf);	// put buffer into stream
	return filter.m_pFunc[iFuncId]((PEER_CLIENT)this, stream);
}
#include "packet_dispatch.h"
#include "packet.h"

DispatchFilter DispatchFilterArray::m_arrDispatchFilter[DISPATCH_FILTER_MAX] = {0};

DispatchFilter& DispatchFilterArray::GetFilter(uint8 iFilterId)
{
	return m_arrDispatchFilter[iFilterId];
}

bool Receiver::OnPacketReceived(void* pSession, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	uint8 iFilterId = (uint8)(iTypeId >> 8);
	uint8 iFuncType = (uint8)(iTypeId & 0xff);

	SN_LOG_DBG(_T("FilterId=%d FuncType=%d Len=%d"), iFilterId, iFuncType, iLen);
	DispatchFilter& filter = DispatchFilterArray::GetFilter(iFilterId);
	// check if func type is valid
	_ASSERT(iFuncType < filter.m_iFuncCount);
	if (iFuncType >= filter.m_iFuncCount)
	{
		SN_LOG_ERR(_T("invalid funcType=%d, funcCount=%d"), iFuncType, filter.m_iFuncCount);
		return false;
	}

	InputStream stream(iLen, pBuf);	// put buffer into stream
	return filter.m_pFunc[iFuncType](pSession, stream);
}

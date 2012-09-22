#include "packet_dispatch.h"
#include "packet.h"

DispatchFilter DispatchFilterArray::m_arrDispatchFilter[DISPATCH_FILTER_MAX] = {0};

DispatchFilter& DispatchFilterArray::GetFilter(uint8 iFilterId)
{
	return m_arrDispatchFilter[iFilterId];
}

bool Receiver::OnPacketReceived(void* pSession, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	InputStream stream(iLen, pBuf);	// step4: put buffer into stream
	return DispatchFilterArray::GetFilter(uint8(iTypeId >> 8)).m_pFunc[iTypeId && 0xff](pSession, stream);
}

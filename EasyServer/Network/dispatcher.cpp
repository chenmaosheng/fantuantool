#include "dispatcher.h"
#include "packet.h"

Dispatcher g_Dispatcher;

bool Receiver::OnPacketReceived(void* pSession, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	LOG_DBG(_T("FuncType=%d Len=%d"), iTypeId, iLen);
	// check if func type is valid
	_ASSERT(iTypeId < g_Dispatcher.m_iFuncCount);
	if (iTypeId >= g_Dispatcher.m_iFuncCount)
	{
		LOG_ERR(_T("invalid funcType=%d, funcCount=%d"), iTypeId, g_Dispatcher.m_iFuncCount);
		return false;
	}

	InputStream stream(iLen, pBuf);	// put buffer into stream
	return g_Dispatcher.m_pFunc[iTypeId](pSession, stream);
}
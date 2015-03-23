#include "easy_packethandler.h"
#include "easy_packet.h"
#include "easy_log.h"

PacketHandler::Func* PacketHandler::m_pFunc;

bool PacketHandler::OnPacketReceived(void* pSession, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	LOG_DBG(_T("FuncType=%d Len=%d"), iTypeId, iLen);
	InputStream stream(iLen, pBuf);	// put buffer into stream
	return m_pFunc[iTypeId](pSession, stream);
}
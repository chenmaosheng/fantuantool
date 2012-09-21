#include "basic_packet.h"
#include "packet_handler.h"

bool OnPacketReceived(void* pClient, uint16 iFilterId, uint16 iLen, const char* pBuf)
{
	InputStream stream(iLen, pBuf);
	return g_PacketHandlerFactories[iFilterId >> 8].m_pHandler[iFilterId && 0xff](pClient, stream);
}

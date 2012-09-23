#ifndef _H_PEER_PACKET
#define _H_PEER_PACKET

#include "common.h"

DECLARE_HANDLE(PEER_SERVER);
DECLARE_HANDLE(PEER_CLIENT);

struct PeerPacket
{
	uint16 m_iFilterId;
	uint16 m_iFuncId;
	uint32 m_iLen;
	char m_Buf[MAX_PEER_BUFFER];
};

#define PEER_PACKET_HEAD sizeof(uint16)*2 + sizeof(uint32)



#endif

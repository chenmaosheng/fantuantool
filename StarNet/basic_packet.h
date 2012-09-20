#ifndef _H_BASIC_PACKET
#define _H_BASIC_PACKET

#include "common.h"

struct ServerPacket
{
	uint16 m_iLen;
	uint16 m_iFilterId;
	char m_Buf[1];
};

#define SERVER_PACKET_HEAD sizeof(uint16) + sizeof(uint16)

#endif

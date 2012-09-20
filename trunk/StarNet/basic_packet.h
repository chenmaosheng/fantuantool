#ifndef _H_BASIC_PACKET
#define _H_BASIC_PACKET

#include "common.h"

struct RawPacket
{
	uint16 m_iLen;	// the whole length of packet, include this variable itself
	char m_Buf[1];
};

struct ServerPacket
{
	uint16 m_iLen;
	uint16 m_iFilterId;
	char m_Buf[1];
};

#define SERVER_PACKET_HEAD sizeof(uint16) + sizeof(uint16)

#endif

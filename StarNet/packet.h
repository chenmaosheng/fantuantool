#ifndef _H_BASIC_PACKET
#define _H_BASIC_PACKET

#include "common.h"

struct ServerPacket
{
	uint16 m_iTypeId; // 1st byte: filterId; 2nd byte: funcType
	uint16 m_iLen;
	char m_Buf[1];
};

#define SERVER_PACKET_HEAD sizeof(uint16) + sizeof(uint16)

class Receiver
{
public:
	static bool OnPacketReceived(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

class Sender
{
public:
	// must implement at app layer
	static int32 SendPacket(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif

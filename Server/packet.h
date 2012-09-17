#ifndef _H_PACKET
#define _H_PACKET

#include "common.h"

struct Packet
{
	uint16 m_iLen;
	char m_Buf[1];
};

struct CS_Packet
{
	uint16 m_iLen;
	uint16 m_iFilterId;
	char m_Buf[1];
};

#endif

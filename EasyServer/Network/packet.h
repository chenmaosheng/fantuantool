#ifndef _H_PACKET
#define _H_PACKET

#include "common.h"

struct Packet
{
	uint16 m_iTypeId;	// 1st byte: filterId; 2nd byte: funcType
	uint16 m_iLen;		// buffer length, not include typeid and length itself
	char m_Buf[1];		// the 1st byte of buffer
};

#define PACKET_HEAD sizeof(uint16) + sizeof(uint16)

#endif

#ifndef _H_EASY_PACKET
#define _H_EASY_PACKET

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EasyPacket
{
	uint16 m_iTypeId;	// 1st byte: filterId; 2nd byte: funcType
	uint16 m_iLen;		// buffer length, not include typeid and length itself
	char m_Buf[1];		// the 1st byte of buffer
}EasyPacket;

#define PACKET_HEAD sizeof(uint16) + sizeof(uint16)

#ifdef __cplusplus
}
#endif

#endif

/*****************************************************************************************
	filename:	packet.h
	created:	09/27/2012
	author:		chen
	purpose:	basic packet info, only for C/S

*****************************************************************************************/

#ifndef _H_PACKET
#define _H_PACKET

#include "starnet_common.h"

struct ServerPacket
{
	uint16 m_iTypeId;	// 1st byte: filterId; 2nd byte: funcType
	uint16 m_iLen;		// buffer length, not include typeid and length itself
	char m_Buf[1];		// the 1st byte of buffer
};

#define SERVER_PACKET_HEAD sizeof(uint16) + sizeof(uint16)

class Receiver
{
public:
	// receive data from client
	static bool OnPacketReceived(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

class Sender
{
public:
	// sendpacket from server to client, must implement at app layer
	// different server may have different implementation
	static int32 SendPacket(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif

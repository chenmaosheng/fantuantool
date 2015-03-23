#ifndef _H_EASY_PACKET_HANDLER
#define _H_EASY_PACKET_HANDLER

#include "easy_stream.h"

class PacketHandler
{
public:
	// receive data from client
	static bool OnPacketReceived(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
	static int32 SendPacket(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);

public:
	typedef bool (CALLBACK* Func)(void*, InputStream&);
	static Func* m_pFunc;
};

#endif

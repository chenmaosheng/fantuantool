#ifndef _H_DISPATCHER
#define _H_DISPATCHER

#include "stream.h"

struct Dispatcher
{
	typedef bool (CALLBACK* Func)(void*, InputStream&);
	uint32 m_iFuncCount;
	Func* m_pFunc;
};

extern Dispatcher g_Dispatcher;

class Receiver
{
public:
	// receive data from client
	static bool OnPacketReceived(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

class Sender
{
public:
	static int32 SendPacket(void*, uint16 iTypeId, uint16 iLen, const char* pBuf);
};

#endif

#ifndef _H_CLIENT_MESSAGE
#define _H_CLIENT_MESSAGE

#include "common.h"
#include "packet.h"
#include "dispatcher.h"

class ClientSend
{
public:
	static int32 PingReq(void* pServer, uint32 iVersion)
	{
		OutputStream stream;
		if (!stream.Serialize(iVersion)) return -1;
		Sender::SendPacket(pServer, 0, stream.GetDataLength(), stream.GetBuffer());
		return 0;
	}
};

class ClientRecv
{
public:
	static void PingAck(void* pClient, uint32 iVersion)
	{
	}

	static bool CALLBACK PingAck_Callback(void* pClient, InputStream& stream)
	{
		uint32 iVersion;
		if (!stream.Serialize(iVersion)) return false;
		ClientRecv::PingAck(pClient, iVersion);
		return true;
	}
};

static Dispatcher::Func func[] = 
{
	ClientRecv::PingAck_Callback,
	NULL
};

struct ClientDispatcher
{
	ClientDispatcher()
	{
		g_Dispatcher.m_pFunc = func;
		g_Dispatcher.m_iFuncCount = sizeof(func)/sizeof(func[0]);
	}
};

static ClientDispatcher clientDispatcher;

#endif

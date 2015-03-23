#ifndef _H_CLIENT_MESSAGE
#define _H_CLIENT_MESSAGE

#include "common.h"
#include "easy_packet.h"
#include "easy_packethandler.h"

class ClientSend
{
public:
	static int32 PingReq(void* pServer, uint32 iVersion)
	{
		OutputStream stream;
		if (!stream.Serialize(iVersion)) return -1;
		PacketHandler::SendPacket(pServer, 0, stream.GetDataLength(), stream.GetBuffer());
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

static PacketHandler::Func func[] = 
{
	ClientRecv::PingAck_Callback,
	NULL
};

struct ClientDispatcher
{
	ClientDispatcher()
	{
		PacketHandler::m_pFunc = func;
	}
};

static ClientDispatcher clientDispatcher;

#endif

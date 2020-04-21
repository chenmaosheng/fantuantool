#ifndef _H_CLIENT_MESSAGE
#define _H_CLIENT_MESSAGE

#include "common.h"
#include "easy_packet.h"
#include "easy_packethandler.h"
#include "easy_log.h"
#include "util.h"

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

	static int32 ChatReq(void* pServer, uint32 iLen, char* message)
	{
		OutputStream stream;
		if (!stream.Serialize(iLen)) return -1;
		if (!stream.Serialize(iLen, message)) return -1;
		PacketHandler::SendPacket(pServer, 1, stream.GetDataLength(), stream.GetBuffer());
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

	static void ChatAck(void* pClient, uint32 len, char* message)
	{
		TCHAR szMessage[128] = {0};
		Char2WChar(message, szMessage, 128);
		LOG_DBG(_T("len=%d, message=%s"), len, szMessage);
	}

	static bool CALLBACK ChatAck_Callback(void* pClient, InputStream& stream)
	{
		char message[1024] = {0};
		uint32 len = 0;
		if (!stream.Serialize(len)) return false;
		if (!stream.Serialize(len, message)) return false;
		ClientRecv::ChatAck(pClient, len, message);
		return true;
	}
};

static PacketHandler::Func func[] = 
{
	ClientRecv::PingAck_Callback,
	ClientRecv::ChatAck_Callback,
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

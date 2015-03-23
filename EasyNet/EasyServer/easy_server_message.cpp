#include "easy_server_message.h"
#include "easy_packet.h"
#include "easy_packethandler.h"
#include "easy_session.h"

bool CALLBACK PingReq_Callback(void* pClient, InputStream& stream)
{
	EasySession* pSession = (EasySession*)pClient;
	uint32 iVersion;
	if (!stream.Serialize(iVersion)) return false;
	pSession->OnPingReq(iVersion);
	return true;
}

static PacketHandler::Func func[] = 
{
	PingReq_Callback,
};

struct ClientDispatcher
{
	ClientDispatcher()
	{
		PacketHandler::m_pFunc = func;
	}
};

static ClientDispatcher clientDispatcher;

int32 PingAck(void* pServer, uint32 iVersion)
{
	OutputStream stream;
	if (!stream.Serialize(iVersion)) return -1;
	PacketHandler::SendPacket(pServer, 0, stream.GetDataLength(), stream.GetBuffer());
	return 0;
}

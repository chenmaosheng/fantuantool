#include "easy_server_message.h"
#include "packet.h"
#include "dispatcher.h"
#include "easy_session.h"

void PingReq(void* pClient, uint32 iVersion)
{
	EasySession* pSession = (EasySession*)pClient;
	pSession->OnPingReq(iVersion);
}

bool CALLBACK PingReq_Callback(void* pClient, InputStream& stream)
{
	uint32 iVersion;
	if (!stream.Serialize(iVersion)) return false;
	PingReq(pClient, iVersion);
	return true;
}

static Dispatcher::Func func[] = 
{
	PingReq_Callback,
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

int32 PingAck(void* pServer, uint32 iVersion)
{
	OutputStream stream;
	if (!stream.Serialize(iVersion)) return -1;
	Sender::SendPacket(pServer, 0, stream.GetDataLength(), stream.GetBuffer());
	return 0;
}


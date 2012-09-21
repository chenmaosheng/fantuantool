#include "test_packet_handler.h"
#include "packet_handler.h"
#include "session.h"
#include "basic_packet.h"
#include <malloc.h>

bool CALLBACK LoginReq_Callback(void* pClient, InputStream& stream)
{
	uint16 iLength = 0;
	stream.Serialize(iLength);
	char* nickname = (char*)alloca(iLength + 1);
	stream.Serialize(iLength, nickname);
	nickname[iLength] = '\0';

	LoginReq(pClient, nickname);

	return true;
}

static PacketHandlerFactory::Handler test_func[] = 
{
	LoginReq_Callback,
	NULL
};

struct Test_PacketHandlerFactory
{
	Test_PacketHandlerFactory()
	{
		g_PacketHandlerFactories[0].m_pHandler = test_func;
	}
};

static Test_PacketHandlerFactory _Test_PacketHandlerFactory;

void LoginReq(void* pClient, const char* strNickname)
{
	Session* pSession = (Session*)pClient;
	pSession->LoginReq(strNickname);
}

int32 LoginNtf(void* pClient, uint32 iSessionId, const char* strNickName)
{
	OutputStream stream;
	stream.Serialize(iSessionId);
	uint16 iLength = (uint16)strlen(strNickName);
	stream.Serialize(iLength);
	stream.Serialize(iLength, strNickName);

	return SendPacket(pClient, 1, stream.GetDataLength(), stream.GetBuffer());
}
#include "test_packet_handler.h"
#include "packet_handler.h"
#include "session.h"
#include <malloc.h>

bool CALLBACK LoginNtf_Callback(void* pClient, InputStream& stream)
{
	uint16 iLength = 0;
	stream.Serialize(iLength);
	char* nickname = (char*)alloca(iLength + 1);
	stream.Serialize(iLength, nickname);
	nickname[iLength] = '\0';

	LoginNtf(pClient, nickname);

	return true;
}

static PacketHandlerFactory::Handler test_func[] = 
{
	LoginNtf_Callback,
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

void LoginNtf(void* pClient, const char* strNickname)
{
	Session* pSession = (Session*)pClient;
	pSession->LoginNtf(strNickname);
}
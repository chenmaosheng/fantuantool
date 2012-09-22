#include "test_client_recv.h"
#include "packet_dispatch.h"
#include "test_packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

bool CALLBACK LoginReq_Callback(void* pClient, InputStream& stream)
{
	uint16 iLength = 0;
	stream.Serialize(iLength);
	char* nickname = (char*)alloca(iLength + 1);
	stream.Serialize(iLength, nickname);
	nickname[iLength] = '\0';		// step5: analyze datastream

	Test_Client_Recv::LoginReq(pClient, nickname);	// step6: call rpc function with parameters

	return true;
}

static DispatchFilter::Func test_func[] = 
{
	LoginReq_Callback,
	NULL
};

Test_PacketDispatch::Test_PacketDispatch()
{
	DispatchFilterArray::GetFilter(0).m_pFunc = test_func;
}


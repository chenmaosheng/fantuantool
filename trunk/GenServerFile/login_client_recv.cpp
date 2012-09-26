#include "login_client_recv.h"
#include "packet_dispatch.h"
#include "login_packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

bool CALLBACK LoginReq_Callback(void* pClient, InputStream& stream)
{
	uint16 iLength = 0;
	stream.Serialize(iLength);
	char* nickname = (char*)_malloca(iLength + 1);
	stream.Serialize(iLength, nickname);
	nickname[iLength] = '\0';		// step5: analyze datastream

	LoginClientRecv::LoginReq(pClient, nickname);	// step6: call rpc function with parameters

	_freea(nickname);
	return true;
}

static DispatchFilter::Func test_func[] = 
{
	LoginReq_Callback,
	NULL
};

LoginPacketDispatch::LoginPacketDispatch()
{
	DispatchFilterArray::GetFilter(0).m_pFunc = test_func;
}

static LoginPacketDispatch _LoginPacketDispatch;
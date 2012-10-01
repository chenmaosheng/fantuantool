#include "login_client_recv.h"
#include "packet_dispatch.h"
#include "login_packet_dispatch.h"
#include "packet.h"
#include <malloc.h>

bool CALLBACK VersionReq_Callback(void* pClient, InputStream& stream)
{
	uint32 iVersion = 0;
	stream.Serialize(iVersion);// step5: analyze datastream

	LoginClientRecv::VersionReq(pClient, iVersion);	// step6: call rpc function with parameters

	return true;
}

static DispatchFilter::Func func[] = 
{
	VersionReq_Callback,
	NULL
};

LoginPacketDispatch::LoginPacketDispatch()
{
	DispatchFilterArray::GetFilter(CLIENT_FILTER_LOGIN).m_pFunc = func;
	DispatchFilterArray::GetFilter(CLIENT_FILTER_LOGIN).m_iFuncCount = sizeof(func)/sizeof(func[0]);
}

static LoginPacketDispatch _LoginPacketDispatch;
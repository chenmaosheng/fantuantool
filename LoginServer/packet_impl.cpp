#include "login_session.h"
#include "login_client_recv.h"

void LoginClientRecv::VersionReq(void* pClient, uint32 iVersion)
{
	LoginSession* pSession = (LoginSession*)pClient;
	pSession->OnVersionReq(iVersion);
}
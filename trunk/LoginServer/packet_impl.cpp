#include "login_session.h"
#include "login_client_recv.h"

void LoginClientRecv::LoginReq(void* pClient, const char* strNickname)
{
	LoginSession* pSession = (LoginSession*)pClient;
	//pSession->LoginReq(strNickname);
}
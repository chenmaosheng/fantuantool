#include "session.h"
#include "test_client_recv.h"

void Test_Client_Recv::LoginReq(void* pClient, const char* strNickname)
{
	Session* pSession = (Session*)pClient;
	pSession->LoginReq(strNickname);
}
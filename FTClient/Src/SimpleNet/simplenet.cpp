#include "simplenet.h"

int32 SimpleNet::Init()
{
	WSADATA wsd;
	SOCKET s;
	
	// start WSA
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		SN_LOG_ERR(_T("WSAStartup failed"));
		return -1;
	}

	// create a socket to initialize asynchorous operation function pointer, need to close when finished
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		SN_LOG_ERR(_T("Create socket failed"));
		return -2;
	}
	
	SN_LOG_STT(_T("Intialize simplenet success"));
	return 0;
}

void SimpleNet::Destroy()
{
	WSACleanup();
}

#include "easy_net.h"

#ifdef __cplusplus
extern "C" {
#endif

LPFN_ACCEPTEX acceptex_;
LPFN_CONNECTEX connectex_;
LPFN_DISCONNECTEX disconnectex_;
LPFN_GETACCEPTEXSOCKADDRS getacceptexsockaddrs_;

int32 InitNet()
{
	WSADATA wsd;
	SOCKET s;
	DWORD cb;
	GUID GuidAcceptEx = WSAID_ACCEPTEX, GuidConnectEx = WSAID_CONNECTEX, GuidDisconnectEx = WSAID_DISCONNECTEX, GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

	// start WSA
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		LOG_ERR(_T("WSAStartup failed"));
		return -1;
	}

	// create a socket to initialize asynchorous operation function pointer, need to close when finished
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_ASSERT(s != INVALID_SOCKET);
	if (s == INVALID_SOCKET)
	{
		LOG_ERR(_T("Create socket failed"));
		return -2;
	}
	
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &acceptex_, sizeof(acceptex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &connectex_, sizeof(connectex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx), &disconnectex_, sizeof(disconnectex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidGetAcceptExSockaddrs, sizeof(GuidGetAcceptExSockaddrs), &getacceptexsockaddrs_, sizeof(getacceptexsockaddrs_), &cb, NULL, NULL);
	closesocket(s);

	LOG_STT(_T("Intialize Network success"));
	return 0;
}

void DestroyNet()
{
	WSACleanup();
}

#ifdef __cplusplus
}
#endif
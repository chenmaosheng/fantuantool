#include "starnet.h"

LPFN_ACCEPTEX StarNet::acceptex_;
LPFN_CONNECTEX StarNet::connectex_;
LPFN_DISCONNECTEX StarNet::disconnectex_;

int32 StarNet::Init()
{
	WSADATA			wsd;
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		return -1;
	}

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		return -2;
	}
	
	DWORD cb;
	GUID GuidAcceptEx = WSAID_ACCEPTEX, GuidConnectEx = WSAID_CONNECTEX, GuidDisconnectEx = WSAID_DISCONNECTEX;
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &acceptex_, sizeof(acceptex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &connectex_, sizeof(connectex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx), &disconnectex_, sizeof(disconnectex_), &cb, NULL, NULL);
	closesocket(s);

	return 0;
}

void StarNet::Destroy()
{
	WSACleanup();
}

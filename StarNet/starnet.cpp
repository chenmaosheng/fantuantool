#include "starnet.h"
#include "peer_server.h"
#include "peer_client.h"

LPFN_ACCEPTEX StarNet::acceptex_;
LPFN_CONNECTEX StarNet::connectex_;
LPFN_DISCONNECTEX StarNet::disconnectex_;
LPFN_GETACCEPTEXSOCKADDRS StarNet::getacceptexsockaddrs_;

int32 StarNet::Init()
{
	WSADATA wsd;
	SOCKET s;
	DWORD cb;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		SN_LOG_ERR(_T("WSAStartup failed"));
		return -1;
	}

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		SN_LOG_ERR(_T("Create socket failed"));
		return -2;
	}
	
	GUID GuidAcceptEx = WSAID_ACCEPTEX, GuidConnectEx = WSAID_CONNECTEX, GuidDisconnectEx = WSAID_DISCONNECTEX, GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &acceptex_, sizeof(acceptex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &connectex_, sizeof(connectex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx), &disconnectex_, sizeof(disconnectex_), &cb, NULL, NULL);
	WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidGetAcceptExSockaddrs, sizeof(GuidGetAcceptExSockaddrs), &getacceptexsockaddrs_, sizeof(getacceptexsockaddrs_), &cb, NULL, NULL);
	closesocket(s);

	return 0;
}

void StarNet::Destroy()
{
	WSACleanup();
}

bool StarNet::StartPeerServer(uint32 iIP, uint16 iPort, uint32 iWorkerCount)
{
	return PeerClientSet::Instance()->Init(iIP, iPort);
}

void StarNet::StopPeerServer()
{
	PeerClientSet::Instance()->Destroy();
}

PEER_SERVER StarNet::GetPeerServer(uint32 iIP, uint16 iPort)
{
	return (PEER_SERVER)PeerServerSet::Instance()->GetPeerServer(iIP, iPort);
}
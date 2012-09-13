#include "common.h"
#include <cstdio>
#include <vector>
#include <process.h>
#include "Command.h"
#include "worker.h"

int __cdecl main(int argc, char **argv)
{
	WSADATA			wsd;
	SOCKET			s;
	int				rc;
	SOCKADDR_IN		addr;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		fprintf(stderr, "unable to load Winsock!\n");
		return -1;
	}

	Worker* pWorker = new Worker;
	pWorker->Init();

	// create the socket
	s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	// bind the socket to a local address and port
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5151);
	rc = bind(s, (sockaddr*)&addr, sizeof(addr));

	printf("Listen to port: %d\n", 5151);

	rc = listen(s, 5);

	while (true)
	{
		SOCKADDR_IN clientAddr;
		SOCKET sock;
		int clientAddrLen = sizeof(clientAddr);
		sock = WSAAccept(s, (SOCKADDR *)&clientAddr, &clientAddrLen, NULL, NULL);
		if (sock == INVALID_SOCKET)
		{
			fprintf(stderr, "accept failed: %d\n", WSAGetLastError());
			return -1;
		}

		Connection* client = new Connection;
		client->socket_ = sock;
		client->connected_ = 1;
		client->sockAddr_ = clientAddr;

		pWorker->clients.push_back(client);
		printf("new client connected, addr=%s\n", inet_ntoa(clientAddr.sin_addr));

		CreateIoCompletionPort((HANDLE)client->socket_, pWorker->iocp_, (ULONG_PTR)client, 0);
		client->AsyncRecv();
	}
	
	WSACleanup();
	return 0;
}

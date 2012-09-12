#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <process.h>
#include "connection.h"
#include "..\Common\Command.h"

#pragma   comment(lib,   "ws2_32.lib")  

HANDLE ghCompletionPort;
std::vector<Connection*> clients;

unsigned int WINAPI WorkerThread(void*);

void DeleteClient(SOCKET sock)
{
	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->socket == sock)
		{
			delete (*it);
			clients.erase(it);
			break;
		}
	}
}

void SendToAll(char* buf, int len)
{
	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->Send(len, buf);
	}
}

int __cdecl main(int argc, char **argv)
{
	WSADATA			wsd;
	SOCKET			s;
	int				rc;
	SOCKADDR_IN		addr;
	int				i = 0;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		fprintf(stderr, "unable to load Winsock!\n");
		return -1;
	}

	// create the socket
	s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	// bind the socket to a local address and port
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5151);
	rc = bind(s, (sockaddr*)&addr, sizeof(addr));

	printf("Listen to port: %d\n", 5151);

	ghCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	HANDLE hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &WorkerThread, NULL, 0, NULL);
	CloseHandle(hWorkerThread);

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
		client->index = ++i;
		client->socket = sock;

		clients.push_back(client);
		printf("new client connected, index=%d\n", client->index);

		CreateIoCompletionPort((HANDLE)client->socket, ghCompletionPort, (ULONG_PTR)client, 0);
		client->Recv();
	}

	
	WSACleanup();
	return 0;
}

unsigned int WINAPI WorkerThread(void*)
{
	BOOL bResult;
	DWORD dwNumRead;
	Connection* pConnection;
	LPOVERLAPPED lpOverlapped = NULL;

	while (true)
	{
		bResult = GetQueuedCompletionStatus(ghCompletionPort, &dwNumRead, (ULONG_PTR*)&pConnection, &lpOverlapped, INFINITE);
		if (lpOverlapped)
		{
			Context* pContext = CtxOfOlap(lpOverlapped);
			switch(pContext->operation)
			{
			case RECV:
				{
					if (dwNumRead == 0)
					{
						printf("client %d disconnected\n", pConnection->index);

						LogoutPkt pkt;
						pkt.index = pConnection->index;
						pkt.len = sizeof(pkt.index);
						SendToAll((char*)&pkt, pkt.len + sizeof(Header));
						DeleteClient(pConnection->socket);
						closesocket(pConnection->socket);
					}
					else
					{
						printf("client %d: %s\n", pConnection->index, pContext->buffer);
						Header* header = (Header*)pContext->buffer;
						if (header->type == LOGIN)
						{
							LoginPkt* pkt = (LoginPkt*)header;
							pkt->index = pConnection->index;
							strncpy(pConnection->nickname, pkt->nickname, sizeof(pConnection->nickname));
							for (size_t i = 0; i < clients.size(); ++i)
							{
								if (clients.at(i)->socket != pConnection->socket)
								{
									LoginPkt newPkt;
									strncpy(newPkt.nickname, clients.at(i)->nickname, sizeof(newPkt.nickname));
									newPkt.nickname[strlen(clients.at(i)->nickname)+1] = '\0';
									newPkt.len = (int)strlen(newPkt.nickname) + sizeof(newPkt.index);
									newPkt.index = clients.at(i)->index;
									pConnection->Send(newPkt.len + sizeof(Header), (char*)&newPkt);
									printf("send to %d\n", pConnection->socket);
								}
							}

							SendToAll((char*)pkt, header->len + sizeof(Header));
						}
						else
						{
							SendToAll(pContext->buffer, header->len + sizeof(Header));
						}

						pConnection->Recv();
					}
					
				}
				break;

			case SEND:
				{
					
				}
				break;
			default:
				break;
			}
		}

	}
}
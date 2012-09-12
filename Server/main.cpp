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
std::vector< std::pair<Connection*, std::string > > nicknames;

unsigned int WINAPI WorkerThread(void*);

std::string GetNickName(Connection* pConnection)
{
	std::vector< std::pair<Connection*, std::string > >::iterator it = nicknames.begin();
	while (it != nicknames.end())
	{
		if ((*it).first == pConnection)
		{
			return (*it).second;
		}

		++it;
	}

	return "";
}
void DeleteClient(SOCKET sock)
{
	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->socket_ == sock)
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
		(*it)->AsyncSend(len, buf);
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
		client->socket_ = sock;
		client->connected_ = 1;
		client->sockAddr_ = clientAddr;

		clients.push_back(client);
		printf("new client connected, addr=%s\n", inet_ntoa(clientAddr.sin_addr));

		CreateIoCompletionPort((HANDLE)client->socket_, ghCompletionPort, (ULONG_PTR)client, 0);
		client->AsyncRecv();
	}

	
	WSACleanup();
	return 0;
}

unsigned int WINAPI WorkerThread(void*)
{
	BOOL bResult;
	DWORD dwNumRead;
	Connection* pConnection = NULL;;
	LPOVERLAPPED lpOverlapped = NULL;
	Context* pContext = NULL;

	while (true)
	{
		bResult = GetQueuedCompletionStatus(ghCompletionPort, &dwNumRead, (ULONG_PTR*)&pConnection, &lpOverlapped, INFINITE);
		if (lpOverlapped)
		{
			pContext = (Context*)((char*)lpOverlapped - CTXOFFSET);
			switch(pContext->operation_type_)
			{
			case OPERATION_RECV:
				{
					if (dwNumRead == 0)
					{
						printf("client %s disconnected\n", inet_ntoa(pConnection->sockAddr_.sin_addr));
						
						LogoutPkt pkt;
						pkt.connID = (int)pConnection;
						pkt.len = sizeof(pkt.connID);
						
						DeleteClient(pConnection->socket_);
						closesocket(pConnection->socket_);

						SendToAll((char*)&pkt, pkt.len + sizeof(Header));
					}
					else
					{
						printf("client %s send something\n", inet_ntoa(pConnection->sockAddr_.sin_addr));
						Header* header = (Header*)pContext->buffer_;
						if (header->type == LOGIN)
						{
							LoginPkt* pkt = (LoginPkt*)header;
							nicknames.push_back( std::pair<Connection*, std::string>(pConnection, pkt->nickname) );
							for (size_t i = 0; i < clients.size(); ++i)
							{
								if (clients.at(i)->socket_ != pConnection->socket_)
								{
									LoginPkt newPkt;
									strcpy_s(newPkt.nickname, sizeof(newPkt.nickname), GetNickName(clients.at(i)).c_str());
									newPkt.len = (int)strlen(newPkt.nickname) + sizeof(newPkt.connID);
									newPkt.connID = (int)(clients.at(i));
									pConnection->AsyncSend(newPkt.len + sizeof(Header), (char*)&newPkt);
									printf("send to %d\n", pConnection->socket_);
								}
							}

							SendToAll((char*)pkt, header->len + sizeof(Header));
						}
						else
						{
							SendToAll(pContext->buffer_, header->len + sizeof(Header));
						}

						pConnection->AsyncRecv();
					}
					
				}
				break;

			case OPERATION_SEND:
				{
					
				}
				break;
			default:
				break;
			}
		}

	}
}
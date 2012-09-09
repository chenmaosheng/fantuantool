// define this before include winsock2.h to up the allowed size
#define FD_SET_SIZE     1024

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "connection.h"

#pragma   comment(lib,   "ws2_32.lib")  

void SendToAll(const std::vector<Connection>& clients, char* buf, size_t size)
{
	std::vector<Connection>::const_iterator it = clients.begin();
	while (it != clients.end())
	{
		send((*it).socket, buf, size, 0);
		++it;
	}
}

int __cdecl main(int argc, char **argv)
{
    WSADATA          wsd;
    SOCKET		     s;
	std::vector<Connection> clients;
	int              rc;
    struct fd_set    fdread, fdsend, fdexcept;
	struct timeval   timeout;
	int				 i = 0;
    
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        fprintf(stderr, "unable to load Winsock!\n");
        return -1;
    }

    // create the socket
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // bind the socket to a local address and port
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5150);
    rc = bind(s, (sockaddr*)&addr, sizeof(addr));
    rc = listen(s, 5);
    
    while (1)
    {
        FD_ZERO(&fdread);
		FD_ZERO(&fdsend);
		FD_ZERO(&fdexcept);
        
        FD_SET(s, &fdread);
		FD_SET(s, &fdsend);
		FD_SET(s, &fdexcept);

		std::vector<Connection>::iterator it = clients.begin();
		while (it != clients.end())
		{
			FD_SET((*it).socket, &fdread);
			FD_SET((*it).socket, &fdsend);
			FD_SET((*it).socket, &fdexcept);

			++it;
		}

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        rc = select(0, &fdread, &fdsend, &fdexcept, &timeout);
        if (rc == SOCKET_ERROR)
        {
            fprintf(stderr, "select failed: %d\n", WSAGetLastError());
            return -1;
        }
        else
        {
            if (FD_ISSET(s, &fdread))
            {
				SOCKADDR_IN clientAddr;
				Connection client;
				client.index = ++i;
				int clientAddrLen = sizeof(clientAddr);
				client.socket = accept(s, (SOCKADDR *)&clientAddr, &clientAddrLen);
				if (client.socket == INVALID_SOCKET)
                {
                    fprintf(stderr, "accept failed: %d\n", WSAGetLastError());
                    return -1;
                }

				clients.push_back(client);
				printf("new client connected, index=%d\n", client.index);
            }

			if (FD_ISSET(s, &fdsend))
			{
			}

			if (FD_ISSET(s, &fdexcept))
			{
				printf("client disconnected\n");
			}

			std::vector<Connection>::iterator it = clients.begin();
			while (it != clients.end())
			{
				bool bErase = false;
				Connection& client = (*it);
				if (FD_ISSET(client.socket, &fdread))
				{
					char buf[128] = {0};
					int ret = recv(client.socket, buf, 128, 0);
					if (ret == 0)
					{
						printf("client %d disconnected\n", client.index);
						closesocket(client.socket);
						it = clients.erase(it);
						bErase = true;
					}
					else
					if (ret == SOCKET_ERROR)
					{
						printf("client %d force disconnected\n", client.index);
						closesocket(client.socket);
						it = clients.erase(it);
						bErase = true;
					}
					else
					{
						printf("client %d: %s\n", client.index, buf);
						//send(client.socket, buf, strlen(buf)+1, 0);
						SendToAll(clients, buf, strlen(buf)+1);
					}
				}

				if (FD_ISSET(client.socket, &fdsend))
				{
				}

				if (FD_ISSET(client.socket, &fdexcept))
				{
					printf("client %d disconnected\n", client.index);
					closesocket(client.socket);
					it = clients.erase(it);
					bErase = true;
				}

				if (!bErase)
				{
					++it;
				}
			}
        }
    }

    WSACleanup();
    return 0;
}

// define this before include winsock2.h to up the allowed size
#define FD_SET_SIZE     1024

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#pragma   comment(lib,   "ws2_32.lib")  



int __cdecl main(int argc, char **argv)
{
    WSADATA          wsd;
    SOCKET		     s;
	SOCKET		     client = INVALID_SOCKET;
    int              rc;
    struct fd_set    fdread, fdsend, fdexcept;
	struct timeval   timeout;
    
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

		if (client != INVALID_SOCKET)
		{
			FD_SET(client, &fdread);
			FD_SET(client, &fdsend);
			FD_SET(client, &fdexcept);
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
				int clientAddrLen = sizeof(clientAddr);
                client = accept(s, (SOCKADDR *)&clientAddr, &clientAddrLen);
                if (client == INVALID_SOCKET)
                {
                    fprintf(stderr, "accept failed: %d\n", WSAGetLastError());
                    return -1;
                }
				printf("new client connected\n");
            }

			if (FD_ISSET(s, &fdsend))
			{
				printf("s what?\n");
			}

			if (FD_ISSET(s, &fdexcept))
			{
				printf("1client disconnected\n");
			}

			if (client != INVALID_SOCKET)
			{
				if (FD_ISSET(client, &fdread))
				{
					char buf[128] = {0};
					int ret = recv(client, buf, 128, 0);
					if (ret == 0)
					{
						closesocket(client);
						client = INVALID_SOCKET;
						printf("client disconnected\n");
					}
					else
					if (ret == SOCKET_ERROR)
					{
						closesocket(client);
						client = INVALID_SOCKET;
						printf("client force disconnected\n");
					}
					else
					{
						printf("client: %s\n", buf);
						send(client, buf, strlen(buf)+1, 0);
					}
				}

				if (FD_ISSET(client, &fdsend))
				{
				}

				if (FD_ISSET(client, &fdexcept))
				{
					printf("client disconnected\n");
				}
			}
        }
    }

    WSACleanup();
    return 0;
}

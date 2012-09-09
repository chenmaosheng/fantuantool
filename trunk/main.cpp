#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#pragma   comment(lib,   "ws2_32.lib")  

unsigned int WINAPI RecvThread(void* s)
{
	SOCKET* recvSocket = (SOCKET*)s;
	int rc = 0;
	char recvBuf[128] = {0};
	while (1)
	{
		rc = recv(*recvSocket, recvBuf, 128, 0);
		if (rc == 0)
		{
			printf("server closed\n");
			break;
		}
		else
		if (rc == SOCKET_ERROR)
		{
			printf("socket error\n");
			break;
		}
		else
		{
			printf("output: %s\n", recvBuf);
		}
	}

	return 0;
}

int __cdecl main(int argc, char **argv)
{
    WSADATA          wsd;
    SOCKET           s;
    int              rc;                            // return code
	SOCKADDR_IN		 addr;
	HANDLE			 recvHandler;
	
	// Load Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        fprintf(stderr, "unable to load Winsock!\n");
        return -1;
    }

    // create the socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
        return -1;
    }

	addr.sin_addr.s_addr = inet_addr("192.168.1.4");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5150);
    rc = connect(s, (SOCKADDR*)&addr, sizeof(addr));
    
	recvHandler = (HANDLE)_beginthreadex(NULL, 0, RecvThread, &s, 0, NULL);

    while (true)
	{
		char buf[128] = {0};
		gets(buf);
		send(s, buf, strlen(buf)+1, 0);
	}

	CloseHandle(recvHandler);
	closesocket(s);

    WSACleanup();
    return 0;
}

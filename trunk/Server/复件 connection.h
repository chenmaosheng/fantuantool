#ifndef _H_CONNECTION
#define _H_CONNECTION

#include <winsock2.h>

enum
{
	ACCEPT,
	SEND,
	RECV,
};

struct Connection
{
	SOCKET socket;
	int index;
	char nickname[64];
	int operation;
	WSAOVERLAPPED	overlapped;
	WSABUF			wsabuf;
	char			buffer[1024];

	void Recv()
	{
		operation = RECV;
		wsabuf.len = 1024;
		wsabuf.buf = buffer;
		DWORD dwXfer, dwFlag = 0;
		WSARecv(socket, &wsabuf, 1, &dwXfer, &dwFlag, &overlapped, NULL);
	}

	void Send(unsigned int len, char* buf)
	{
		operation = SEND;
		wsabuf.len = len;
		strncpy(wsabuf.buf, buf, len);
		DWORD dwXfer;
		WSASend(socket, &wsabuf, 1, &dwXfer, 0, &overlapped, NULL);
	}
};

#endif

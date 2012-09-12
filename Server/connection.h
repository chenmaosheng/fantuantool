#ifndef _H_CONNECTION
#define _H_CONNECTION

#include <winsock2.h>

enum
{
	SEND,
	RECV,
};

struct Connection;
struct Context
{
	int operation;
	WSAOVERLAPPED	overlapped;
	char buffer[1024];
	WSABUF wsabuf;
	Connection* connection;

	Context()
	{
		memset(buffer, 0, sizeof(buffer));
		ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
		wsabuf.buf = buffer;
	}

	void Set()
	{
		ZeroMemory(&overlapped, sizeof(WSAOVERLAPPED));
		wsabuf.buf = buffer;
	}
};

#define	CtxOfOlap(olap)	((Context*)((LPBYTE)(olap)-FIELD_OFFSET(Context, overlapped)))

struct Connection
{
	SOCKET socket;
	int index;
	char nickname[64];
	Context sendContext;
	Context recvContext;

	Connection()
	{
		sendContext.operation = SEND;
		sendContext.connection = this;
		recvContext.operation = RECV;
		recvContext.connection = this;
	}

	void Recv()
	{
		recvContext.Set();
		recvContext.wsabuf.len = 1024;
		DWORD dwXfer, dwFlag = 0;
		if (WSARecv(socket, &recvContext.wsabuf, 1, &dwXfer, &dwFlag, &recvContext.overlapped, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				return;
			}
		}
	}

	void Send(int len, char* buf)
	{
		sendContext.Set();
		memcpy(sendContext.buffer, buf, len);
		sendContext.buffer[strlen(sendContext.buffer)] = '\0';
		sendContext.wsabuf.len = len;
		sendContext.wsabuf.buf = sendContext.buffer;
		DWORD dwXfer;
		if (WSASend(socket, &sendContext.wsabuf, 1, &dwXfer, 0, &sendContext.overlapped, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				return;
			}
		}
	}
};

#endif

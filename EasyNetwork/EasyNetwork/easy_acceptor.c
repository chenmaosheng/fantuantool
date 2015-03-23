#include "easy_acceptor.h"
#include "easy_connection.h"
#include "easy_net.h"
#include <process.h>

#ifdef __cplusplus
extern "C" {
#endif

EasyAcceptor* CreateAcceptor(uint32 ip, uint16 port)
{
	int32 rc = 0;
	DWORD val = 0;
	SOCKADDR_IN addr;
	
	EasyAcceptor* pAcceptor = (EasyAcceptor*)_aligned_malloc(sizeof(EasyAcceptor), MEMORY_ALLOCATION_ALIGNMENT);
	if (pAcceptor)
	{
		// initialize acceptor's tcp socket
		pAcceptor->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		_ASSERT(pAcceptor->socket_ != INVALID_SOCKET);
		if (pAcceptor->socket_ == INVALID_SOCKET)
		{
			LOG_ERR(_T("Create acceptor socket failed, err=%d"), WSAGetLastError());
			return NULL;
		}

		// set snd buf and recv buf to 0, it's said that it must improve the performance
		setsockopt(pAcceptor->socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
		setsockopt(pAcceptor->socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));
		val = 1;
		setsockopt(pAcceptor->socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
		setsockopt(pAcceptor->socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

		LOG_STT(_T("Create and configure acceptor socket"));

		// bind acceptor's socket to assigned ip address
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(ip);
		addr.sin_port = htons(port);
		rc = bind(pAcceptor->socket_, (SOCKADDR*)&addr, sizeof(addr));
		_ASSERT(rc == 0);
		if (rc != 0)
		{
			LOG_ERR(_T("bind to address failed, err=%d"), rc);
			return NULL;
		}

		// set socket into listening for incoming connection
		rc = listen(pAcceptor->socket_, SOMAXCONN);
		_ASSERT(rc == 0);
		if (rc != 0)
		{
			LOG_ERR(_T("listen to the socket, err=%d"), rc);
			return NULL;
		}

		pAcceptor->server_ = NULL;

		LOG_STT(_T("Initialize and start acceptor success"));
	}

	return pAcceptor;
}

void DestroyAcceptor(EasyAcceptor* pAcceptor)
{
	// todo: io may not fully handled
	LOG_STT(_T("Clear all connections in free list"));
	// close the accept socket
	if (pAcceptor->socket_ != INVALID_SOCKET)
	{
		closesocket(pAcceptor->socket_);
	}

	LOG_STT(_T("Destroy acceptor success"));
	_aligned_free(pAcceptor);
}

EasyConnection* AcceptConnection(EasyAcceptor* pAcceptor)
{
	SOCKADDR_IN sinRemote;
	int nAddrSize = sizeof(sinRemote);
	u_long nNoBlock = 1;
	EasyConnection* pConnection = CreateConnection(pAcceptor);
	
	LOG_DBG(_T("Get a new connection and wait for incoming connect"));

	pConnection->client_ = NULL;
	pConnection->socket_ = accept(pAcceptor->socket_, (SOCKADDR*)&sinRemote, &nAddrSize);
	ioctlsocket(pConnection->socket_, FIONBIO, &nNoBlock);
	LOG_DBG(_T("Accept success"));

	return pConnection;
}

#ifdef __cplusplus
}
#endif

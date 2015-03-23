#include "easy_acceptor.h"
#include "easy_connection.h"
#include "easy_net.h"
#include "easy_worker.h"
#include "easy_log.h"

#ifdef __cplusplus
extern "C" {
#endif

EasyAcceptor* CreateAcceptor(uint32 ip, uint16 port, EasyWorker* pWorker, EasyHandler* pHandler)
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

		// initialize the context and set io type to accept
		ZeroMemory(&pAcceptor->context_.overlapped_, sizeof(WSAOVERLAPPED));
		pAcceptor->context_.operation_type_ = OPERATION_ACCEPT;
		
		// bind acceptor's socket to iocp handle
		if (!CreateIoCompletionPort((HANDLE)pAcceptor->socket_, pWorker->iocp_, (ULONG_PTR)pAcceptor, 0))
		{
			_ASSERT(FALSE && _T("CreateIoCompletionPort failed"));
			LOG_ERR(_T("CreateIoCompletionPort failed, err=%d"), WSAGetLastError());
			return NULL;
		}

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

		pAcceptor->worker_ = pWorker;
		pAcceptor->handler_ = *pHandler;
		pAcceptor->server_ = NULL;

		// start acceptor
		AcceptConnection(pAcceptor);

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

void AcceptConnection(EasyAcceptor* pAcceptor)
{
	DWORD dwXfer;
	EasyConnection* pConnection = CreateConnection(&pAcceptor->handler_, pAcceptor->worker_, pAcceptor);
	LOG_DBG(_T("Get a new connection and wait for incoming connect"));

	pConnection->client_ = NULL;
	pAcceptor->context_.connection_ = pConnection;

	// post an asychronous accept
	if (!acceptex_(pAcceptor->socket_, pConnection->socket_, pAcceptor->context_.buffer_, 0, sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16, &dwXfer, &pAcceptor->context_.overlapped_))
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("AcceptEx failed, err=%d"), iLastError);

			CloseConnection(pConnection);
		}
		else
		{
			LOG_DBG(_T("Acceptex pending"));
		}
	}

	LOG_DBG(_T("AcceptEx success"));
}

#ifdef __cplusplus
}
#endif

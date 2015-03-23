#include "acceptor.h"
#include <cstdio>
#include "connection.h"
#include "worker.h"
#include "context.h"
#include "network.h"

int32 Acceptor::Init(PSOCKADDR_IN addr, Worker* pWorker, Handler* pHandler)
{
	int32 rc = 0;
	DWORD val = 0;
	
	running_ = 0;
	
	// initialize acceptor's tcp socket
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_ASSERT(socket_ != INVALID_SOCKET);
	if (socket_ == INVALID_SOCKET)
	{
		LOG_ERR(_T("Create acceptor socket failed, err=%d"), WSAGetLastError());
		return -1;
	}

	// set snd buf and recv buf to 0, it's said that it must improve the performance
	setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
	setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

	val = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
	setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

	LOG_STT(_T("Create and configure acceptor socket"));

	// initialize the context and set io type to accept
	ZeroMemory(&context_.overlapped_, sizeof(WSAOVERLAPPED));
	context_.operation_type_ = OPERATION_ACCEPT;

	// bind acceptor's socket to iocp handle
	if (!CreateIoCompletionPort((HANDLE)socket_, pWorker->iocp_, (ULONG_PTR)this, 0))
	{
		_ASSERT(false && _T("CreateIoCompletionPort failed"));
		LOG_ERR(_T("CreateIoCompletionPort failed, err=%d"), WSAGetLastError());
		return -4;
	}

	// bind acceptor's socket to assigned ip address
	rc = bind(socket_, (sockaddr*)addr, sizeof(*addr));
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		LOG_ERR(_T("bind to address failed, err=%d"), rc);
		return -5;
	}

	// set socket into listening for incoming connection
	rc = listen(socket_, SOMAXCONN);
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		LOG_ERR(_T("listen to the socket, err=%d"), rc);
		return -6;
	}

	worker_ = pWorker;
	handler_ = *pHandler;
	server_ = NULL;

	LOG_STT(_T("Initialize acceptor success"));

	return 0;
}

void Acceptor::Destroy()
{
	// first stop the acceptor
	Stop();

	// todo: io may not fully handled

	LOG_STT(_T("Clear all connections in free list"));

	// close the accept socket
	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);
	}

	LOG_STT(_T("Destroy acceptor success"));
}

void Acceptor::Accept()
{
	Connection* pConnection = Connection::Create(&handler_, worker_, this);
	LOG_DBG(_T("Get a new connection and wait for incoming connect"));

	pConnection->client_ = NULL;
	context_.connection_ = pConnection;

	DWORD dwXfer;

	// post an asychronous accept
	if (!Network::acceptex_(socket_, pConnection->socket_, context_.buffer_, 0, sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16, &dwXfer, &context_.overlapped_))
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("AcceptEx failed, err=%d"), iLastError);

			Connection::Close(pConnection);
			running_ = 0;
		}
		else
		{
			LOG_DBG(_T("Acceptex pending"));
		}
	}

	LOG_DBG(_T("AcceptEx success"));
}

void Acceptor::Start()
{
	// check if running is not 0
	if (!InterlockedCompareExchange(&running_, 1, 0))
	{
		Accept();
	}
}

void Acceptor::Stop()
{
	// set running to 0
	running_ = 0;
}

void Acceptor::SetServer(void* pServer)
{
	server_ = pServer;
}

void* Acceptor::GetServer()
{
	return server_;
}

Acceptor* Acceptor::CreateAcceptor(PSOCKADDR_IN addr, Worker* pWorker, Handler* pHandler)
{
	Acceptor* pAcceptor = (Acceptor*)_aligned_malloc(sizeof(Acceptor), MEMORY_ALLOCATION_ALIGNMENT);
	if (pAcceptor)
	{
		pAcceptor->Init(addr, pWorker, pHandler);
	}

	return pAcceptor;
}

void Acceptor::DestroyAcceptor(Acceptor* pAcceptor)
{
	pAcceptor->Destroy();
	_aligned_free(pAcceptor);
}

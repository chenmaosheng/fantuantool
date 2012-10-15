#include "acceptor.h"
#include <cstdio>
#include "connection.h"
#include "worker.h"
#include "context_pool.h"
#include "context.h"
#include "starnet.h"

int32 Acceptor::Init(PSOCKADDR_IN addr, Worker* pWorker, ContextPool* pContextPool, Handler* pHandler)
{
	int32 rc = 0;
	DWORD val = 0;
	
	iorefs_ = 0;
	running_ = 0;
	total_connection_ = 0;
	
	// initialize acceptor's tcp socket
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_ASSERT(socket_ != INVALID_SOCKET);
	if (socket_ == INVALID_SOCKET)
	{
		SN_LOG_ERR(_T("Create acceptor socket failed, err=%d"), WSAGetLastError());
		return -1;
	}

	// set snd buf and recv buf to 0, it's said that it must improve the performance
	setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
	setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

	val = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
	setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

	SN_LOG_STT(_T("Create and configure acceptor socket"));

	// create slist of free connection
	free_connection_ = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);
	_ASSERT(free_connection_);
	if (!free_connection_)
	{
		SN_LOG_ERR(_T("Allocate SList of free connection failed, err=%d"), GetLastError());
		return -2;
	}

	InitializeSListHead(free_connection_);

	SN_LOG_STT(_T("Create and initialize SList of free connection"));
	
	// create initial acceptor context, necessary
	context_ = (Context*)_aligned_malloc(sizeof(Context), MEMORY_ALLOCATION_ALIGNMENT);
	_ASSERT(context_);
	if (!context_)
	{
		SN_LOG_ERR(_T("Create initial acceptor context failed, err=%d"), GetLastError());
		return -3;
	}

	// initialize the context and set io type to accept
	ZeroMemory(&context_->overlapped_, sizeof(WSAOVERLAPPED));
	context_->operation_type_ = OPERATION_ACCEPT;

	// bind acceptor's socket to iocp handle
	if (!CreateIoCompletionPort((HANDLE)socket_, pWorker->iocp_, (ULONG_PTR)this, 0))
	{
		_ASSERT(false && _T("CreateIoCompletionPort failed"));
		SN_LOG_ERR(_T("CreateIoCompletionPort failed, err=%d"), WSAGetLastError());
		return -4;
	}

	// bind acceptor's socket to assigned ip address
	rc = bind(socket_, (sockaddr*)addr, sizeof(*addr));
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		SN_LOG_ERR(_T("bind to address failed, err=%d"), rc);
		return -5;
	}

	// set socket into listening for incoming connection
	rc = listen(socket_, SOMAXCONN);
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		SN_LOG_ERR(_T("listen to the socket, err=%d"), rc);
		return -6;
	}

	worker_ = pWorker;
	handler_ = *pHandler;
	context_pool_ = pContextPool;
	server_ = NULL;

	SN_LOG_STT(_T("Initialize acceptor success"));

	return 0;
}

void Acceptor::Destroy()
{
	// first stop the acceptor
	Stop();

	// if some io are not released, wait a while
	while (iorefs_)
	{
		Sleep(100);
	}

	// clear all connections in free list
	if (free_connection_)
	{
		while (QueryDepthSList(free_connection_) != total_connection_)
		{
			Sleep(100);
		}

		while (QueryDepthSList(free_connection_))
		{
			Connection::Close((Connection*)InterlockedPopEntrySList(free_connection_));
		}

		_aligned_free(free_connection_);
	}

	SN_LOG_STT(_T("Clear all connections in free list"));

	// close the accept socket
	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);
	}

	SN_LOG_STT(_T("Destroy acceptor success"));
}

void Acceptor::Accept()
{
	// get one connection from free list
	Connection* pConnection = (Connection*)InterlockedPopEntrySList(free_connection_);
	if (!pConnection)
	{
		pConnection = Connection::Create(&handler_, context_pool_, worker_, this);
		_ASSERT(pConnection);
		if (!pConnection)
		{
			SN_LOG_ERR(_T("Create connection failed, err=%d"), GetLastError());
			running_ = 0;
			return;
		}
	}
	else
	{
		total_connection_--;
	}

	SN_LOG_DBG(_T("Get a new connection and wait for incoming connect"));

	pConnection->client_ = NULL;
	context_->connection_ = pConnection;

	InterlockedIncrement(&iorefs_);
	DWORD dwXfer;

	// post an asychronous accept
	if (!StarNet::acceptex_(socket_, pConnection->socket_, context_->buffer_, 0, sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16, &dwXfer, &context_->overlapped_))
	{
		int32 iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			SN_LOG_ERR(_T("AcceptEx failed, err=%d"), iLastError);

			Connection::Close(pConnection);
			InterlockedPushEntrySList(free_connection_, pConnection);
			total_connection_++;
			InterlockedDecrement(&iorefs_);
			running_ = 0;
		}
		else
		{
			SN_LOG_DBG(_T("Acceptex pending"));
		}
	}

	SN_LOG_DBG(_T("AcceptEx success"));
}

void Acceptor::Start()
{
	// check if running is not 0
	if (!InterlockedCompareExchange(&running_, 1, 0))
	{
		// confirm there is no uncomplete io request
		while (iorefs_)
		{
			Sleep(100);
		}

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

Acceptor* Acceptor::CreateAcceptor(PSOCKADDR_IN addr, Worker* pWorker, ContextPool* pContextPool, Handler* pHandler)
{
	Acceptor* pAcceptor = (Acceptor*)_aligned_malloc(sizeof(Acceptor), MEMORY_ALLOCATION_ALIGNMENT);
	if (pAcceptor)
	{
		pAcceptor->Init(addr, pWorker, pContextPool, pHandler);
	}

	return pAcceptor;
}

void Acceptor::DestroyAcceptor(Acceptor* pAcceptor)
{
	pAcceptor->Destroy();
	_aligned_free(pAcceptor);
}

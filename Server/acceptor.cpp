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
	
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_ == INVALID_SOCKET)
	{
		return -1;
	}

	// set snd buf and recv buf to 0, it's said that it must improve the performance
	setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
	setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

	val = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
	setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

	free_connection_ = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);
	if (!free_connection_)
	{
		return -2;
	}

	InitializeSListHead(free_connection_);
	
	context_ = (Context*)_aligned_malloc(sizeof(Context), MEMORY_ALLOCATION_ALIGNMENT);
	if (!context_)
	{
		return -3;
	}
	ZeroMemory(&context_->overlapped_, sizeof(WSAOVERLAPPED));
	context_->operation_type_ = OPERATION_ACCEPT;

	if (!CreateIoCompletionPort((HANDLE)socket_, pWorker->iocp_, (ULONG_PTR)this, 0))
	{
		return -4;
	}

	rc = bind(socket_, (sockaddr*)addr, sizeof(*addr));
	if (rc != 0)
	{
		return -5;
	}

	rc = listen(socket_, SOMAXCONN);
	if (rc != 0)
	{
		return -6;
	}

	worker_ = pWorker;
	handler_ = *pHandler;
	context_pool_ = pContextPool;
	server_ = NULL;

	return 0;
}

void Acceptor::Destroy()
{
	Stop();
	while (iorefs_)
	{
		Sleep(100);
	}

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

	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);
	}
}

void Acceptor::Accept()
{
	Connection* pConnection = (Connection*)InterlockedPopEntrySList(free_connection_);
	if (!pConnection)
	{
		pConnection = Connection::Create(&handler_, context_pool_, worker_, this);
		if (!pConnection)
		{
			running_ = 0;
			return;
		}
	}
	else
	{
		total_connection_--;
	}

	pConnection->client_ = NULL;
	context_->connection_ = pConnection;

	InterlockedIncrement(&iorefs_);
	DWORD dwXfer;

	if (!StarNet::acceptex_(socket_, pConnection->socket_, context_->buffer_, 0, sizeof(SOCKADDR_IN)+16,
		sizeof(SOCKADDR_IN)+16, &dwXfer, &context_->overlapped_))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			Connection::Close(pConnection);
			InterlockedPushEntrySList(free_connection_, pConnection);
			total_connection_++;
			InterlockedDecrement(&iorefs_);
			running_ = 0;
		}
	}
}

void Acceptor::Start()
{
	if (!InterlockedCompareExchange(&running_, 1, 0))
	{
		while (iorefs_)
		{
			Sleep(100);
		}

		Accept();
	}
}

void Acceptor::Stop()
{
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

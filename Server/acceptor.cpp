#include "acceptor.h"
#include <cstdio>
#include "connection.h"
#include "worker.h"
#include "context_pool.h"
#include "context.h"

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

	rc = setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const char*)&val, sizeof(val));
	if (rc != 0)
	{
		return -2;
	}

	rc = setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const char*)&val, sizeof(val));
	if (rc != 0)
	{
		return -3;
	}

	val = 1;
	rc = setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&val, sizeof(val));
	if (rc != 0)
	{
		return -4;
	}

	rc = setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (const char*)&val, sizeof(val));
	if (rc != 0)
	{
		return -5;
	}

	free_connection_ = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER), MEMORY_ALLOCATION_ALIGNMENT);
	if (!free_connection_)
	{
		return -6;
	}

	InitializeSListHead(free_connection_);
	
	context_ = (Context*)_aligned_malloc(sizeof(Context), MEMORY_ALLOCATION_ALIGNMENT);
	if (!context_)
	{
		return -7;
	}
	ZeroMemory(&context_->overlapped_, sizeof(WSAOVERLAPPED));
	context_->operation_type_ = OPERATION_ACCEPT;

	if (!CreateIoCompletionPort((HANDLE)socket_, pWorker->iocp_, (ULONG_PTR)this, 0))
	{
		return -8;
	}

	rc = bind(socket_, (sockaddr*)addr, sizeof(*addr));
	if (rc != 0)
	{
		return -9;
	}

	rc = listen(socket_, SOMAXCONN);
	if (rc != 0)
	{
		return -10;
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
		
	}
	SOCKADDR_IN clientAddr;
	SOCKET sock;
	int clientAddrLen = sizeof(clientAddr);
	sock = accept(socket_, (SOCKADDR *)&clientAddr, &clientAddrLen);
	if (sock == INVALID_SOCKET)
	{
		fprintf(stderr, "accept failed: %d\n", WSAGetLastError());
		return;
	}

	Connection* client = new Connection;
	client->socket_ = sock;
	client->sockaddr_ = clientAddr;
	client->handler_ = handler_;
	//client->context_pool_ = context_pool_;
	CreateIoCompletionPort((HANDLE)client->socket_, worker_->iocp_, (ULONG_PTR)client, 0);

	printf("new client connected, addr=%s\n", inet_ntoa(clientAddr.sin_addr));
	client->handler_.OnConnection((ConnID)client);
	client->connected_ = 1;

	//client->AsyncRecv();
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

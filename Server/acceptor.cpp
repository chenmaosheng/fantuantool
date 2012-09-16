#include "acceptor.h"
#include <cstdio>
#include "connection.h"
#include "worker.h"
#include "context_pool.h"

void Acceptor::Init(PSOCKADDR_IN addr, Worker* pWorker, ContextPool* pContextPool, Handler* pHandler)
{
	int rc = 0;
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	rc = bind(socket_, (sockaddr*)addr, sizeof(*addr));
	rc = listen(socket_, SOMAXCONN);

	worker_ = pWorker;
	handler_ = *pHandler;
	context_pool_ = pContextPool;
}

void Acceptor::Destroy()
{
}

void Acceptor::Accept()
{
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
	client->sockAddr_ = clientAddr;
	client->handler_ = handler_;
	//client->context_pool_ = context_pool_;
	CreateIoCompletionPort((HANDLE)client->socket_, worker_->iocp_, (ULONG_PTR)client, 0);

	printf("new client connected, addr=%s\n", inet_ntoa(clientAddr.sin_addr));
	client->handler_.OnConnection((ConnID)client);
	client->connected_ = 1;

	client->AsyncRecv();
}

void Acceptor::Start()
{
	Accept();
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

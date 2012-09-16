#include "connection.h"
#include "worker.h"
#include "context.h"
#include "context_pool.h"
#include "acceptor.h"
#include "starnet.h"

int32 Connection::Connect(PSOCKADDR_IN addr, void* client)
{
	int32 rc = 0;
	client_ = client;
	context_ = (Context*)_aligned_malloc(sizeof(Context), MEMORY_ALLOCATION_ALIGNMENT);
	if (!context_)
	{
		return -1;
	}

	rc = StarNet::connectex_(socket_, (sockaddr*)addr, sizeof(*addr), NULL, 0, NULL, &context_->overlapped_);
	if (rc == 0)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return -2;
		}
	}

	return 0;
}

void Connection::Disconnect()
{
	int32 rc = 0;
	if (InterlockedCompareExchange(&connected_, 0, 1))
	{
		context_->operation_type_ = OPERATION_DISCONNECT;
		rc = StarNet::disconnectex_(socket_, &context_->overlapped_, acceptor_ ? TF_REUSE_SOCKET: 0, 0);
		if (rc == 0)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				return;
			}
		}
	}
}

void Connection::AsyncSend(Context* pContext)
{
	pContext->connection_ = this;
	if (iorefs_ > iorefmax_)
	{
		context_pool_->PushOutputContext(pContext);
		Disconnect();
	}

	InterlockedIncrement(&iorefs_);
	DWORD dwXfer;
	if (WSASend(socket_, &pContext->wsabuf_, 1, &dwXfer, 0, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			context_pool_->PushOutputContext(pContext);
			Disconnect();
			InterlockedDecrement(&iorefs_);
		}
	}
}

void Connection::AsyncRecv(Context* pContext)
{
	pContext->connection_ = this;
	pContext->wsabuf_.len = context_pool_->input_buffer_size_;
	InterlockedIncrement(&iorefs_);
	DWORD dwXfer, dwFlag = 0;
	if (WSARecv(socket_, &pContext->wsabuf_, 1, &dwXfer, &dwFlag, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			context_pool_->PushInputContext(pContext);
			Disconnect();
			InterlockedDecrement(&iorefs_);
		}
	}
}

Connection* Connection::Create(Handler* pHandler, ContextPool* pContextPool, Worker* pWorker, Acceptor* pAcceptor)
{
	Connection* pConnection = (Connection*)_aligned_malloc(sizeof(Connection), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnection)
	{
		pConnection->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (pConnection->socket_ != INVALID_SOCKET)
		{
			if (CreateIoCompletionPort((HANDLE)pConnection->socket_, pWorker->iocp_, (ULONG_PTR)pConnection, 0))
			{
				
			}
		}
	}

	return pConnection;
}

void Connection::Close(Connection* pConnection)
{
	closesocket(pConnection->socket_);
	_aligned_free(pConnection->context_);
	_aligned_free(pConnection);
}
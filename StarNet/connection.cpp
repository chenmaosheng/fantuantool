#include "connection.h"
#include "worker.h"
#include "context.h"
#include "context_pool.h"
#include "acceptor.h"
#include "starnet.h"

int32 Connection::AsyncConnect(PSOCKADDR_IN addr, void* client)
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

void Connection::AsyncDisconnect()
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
		AsyncDisconnect();
	}

	InterlockedIncrement(&iorefs_);
	DWORD dwXfer;
	if (WSASend(socket_, &pContext->wsabuf_, 1, &dwXfer, 0, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			context_pool_->PushOutputContext(pContext);
			AsyncDisconnect();
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
			AsyncDisconnect();
			InterlockedDecrement(&iorefs_);
		}
	}
}

void Connection::AsyncSend(uint32 len, char* buf)
{
	Context* pContext = (Context*)((char*)buf - BUFOFFSET);
	if (pContext->operation_type_ != OPERATION_SEND)
	{
		return;
	}

	if (pContext->context_pool_->output_buffer_size_ < len)
	{
		return;
	}

	pContext->wsabuf_.len = len;
	AsyncSend(pContext);
}

void Connection::SetClient(void* pClient)
{
	client_ = pClient;
}

void* Connection::GetClient()
{
	return client_;
}

void Connection::SetRefMax(uint16 iMax)
{
	iorefmax_ = iMax;
}

bool Connection::IsConnected()
{
	return connected_ ? true : false;
}

Connection* Connection::Create(Handler* pHandler, ContextPool* pContextPool, Worker* pWorker, Acceptor* pAcceptor)
{
	Connection* pConnection = (Connection*)_aligned_malloc(sizeof(Connection), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnection)
	{
		pConnection->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (pConnection->socket_ != INVALID_SOCKET)
		{
			// the 3rd param is the key of getqueued
			if (CreateIoCompletionPort((HANDLE)pConnection->socket_, pWorker->iocp_, (ULONG_PTR)pConnection, 0))
			{
				DWORD val = 0;

				// set snd buf and recv buf to 0, it's said that it must improve the performance
				setsockopt(pConnection->socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
				setsockopt(pConnection->socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

				val = 1;
				setsockopt(pConnection->socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
				setsockopt(pConnection->socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

				pConnection->context_ = (Context*)_aligned_malloc(sizeof(Context), MEMORY_ALLOCATION_ALIGNMENT);
				if (pConnection->context_)
				{
					pConnection->context_->operation_type_ = OPERATION_ACCEPT;
					pConnection->handler_ = *pHandler;
					pConnection->context_pool_ = pContextPool;
					pConnection->worker_ = pWorker;
					pConnection->acceptor_ = pAcceptor;
					pConnection->context_->connection_ = pConnection;
					pConnection->connected_ = 0;
					pConnection->iorefs_ = 0;
					pConnection->iorefmax_ = 65536;
					ZeroMemory(&pConnection->context_->overlapped_, sizeof(WSAOVERLAPPED));

					return pConnection;
				}
			}
			else
			{
				closesocket(pConnection->socket_);
			}
		}

		_aligned_free(pConnection);
	}

	return NULL;
}

bool Connection::Connect(PSOCKADDR_IN pAddr, Handler* pHandler, ContextPool* pContextPool, Worker* pWorker, void* pClient)
{
	Connection* pConnection = Create(pHandler, pContextPool, pWorker, NULL);
	if (pConnection)
	{
		if (pConnection->AsyncConnect(pAddr, pClient))
		{
			return true;
		}

		Delete(pConnection);
	}

	return false;
}

void Connection::Close(Connection* pConnection)
{
	if (pConnection->iorefs_ || pConnection->connected_)
	{
		return;
	}

	if (pConnection->acceptor_)
	{
		InterlockedPushEntrySList(pConnection->acceptor_->free_connection_, pConnection);
	}
	else
	{
		Delete(pConnection);
	}
}

void Connection::Delete(Connection* pConnection)
{
	closesocket(pConnection->socket_);
	_aligned_free(pConnection->context_);
	_aligned_free(pConnection);
}
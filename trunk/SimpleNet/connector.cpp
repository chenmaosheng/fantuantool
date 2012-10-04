#include "connector.h"
#include "worker.h"

bool Connector::Connect(PSOCKADDR_IN addr, void* client)
{
	int32 rc = 0;
	client_ = client;

	rc = connect(socket_, (sockaddr*)addr, sizeof(*addr));
	if (rc == 0)
	{
		int32 iLastError = WSAGetLastError();
		if (iLastError != ERROR_IO_PENDING)
		{
			SN_LOG_ERR(_T("Connect failed, err=%d"), iLastError);
			return false;
		}
	}

	SN_LOG_DBG(_T("ConnectEx success"));

	return true;
}

void Connector::Disconnect()
{
}

void Connector::Recv()
{
	pContext->connection_ = this;
	pContext->wsabuf_.len = context_pool_->input_buffer_size_;
	InterlockedIncrement(&iorefs_);
	DWORD dwXfer, dwFlag = 0;
	// post an asychronous receive
	if (WSARecv(socket_, &pContext->wsabuf_, 1, &dwXfer, &dwFlag, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		int32 iLastError = WSAGetLastError();
		if (iLastError != ERROR_IO_PENDING)
		{
			SN_LOG_ERR(_T("WSARecv failed, err=%d"), iLastError);
			context_pool_->PushInputContext(pContext);
			AsyncDisconnect();
			InterlockedDecrement(&iorefs_);
		}
	}

	SN_LOG_DBG(_T("WSARecv success"));
}

void Connection::Send(uint32 len, char* buf)
{
	int rc = 0;
	rc = send(socket_, buf, len, 0);
	if (rc == 0)
	{

	}
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
		// initialize connection's tcp socket
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
					pConnection->handler_ = *pHandler;
					pConnection->context_pool_ = pContextPool;
					pConnection->worker_ = pWorker;
					pConnection->acceptor_ = pAcceptor;
					pConnection->context_->connection_ = pConnection;
					pConnection->connected_ = 0;
					pConnection->iorefs_ = 0;
					pConnection->iorefmax_ = 65536;
					ZeroMemory(&pConnection->context_->overlapped_, sizeof(WSAOVERLAPPED));
					if (!pAcceptor)
					{
						// if acceptor=NULL, means it's called at client side
						// connection's socket must bind to it's address
						SOCKADDR_IN addr;
						ZeroMemory(&addr, sizeof(addr));
						addr.sin_family = AF_INET;
						if (bind(pConnection->socket_, (sockaddr*)&addr, sizeof(addr)) == 0)
						{
							SN_LOG_DBG(_T("Create and configure connection in client side"));
							return pConnection;
						}
					}
					else
					{
						SN_LOG_DBG(_T("Create and configure connection in server side"));
						return pConnection;
					}
				}
			}
			else
			{
				SN_LOG_ERR(_T("CreateIoCompletionPort failed, err=%d"), WSAGetLastError());
				closesocket(pConnection->socket_);
			}
		}

		_aligned_free(pConnection);
	}

	SN_LOG_ERR(_T("Create connection failed, err=%d"), GetLastError());

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
	// check if io reference count is 0
	if (pConnection->iorefs_ || pConnection->connected_)
	{
		SN_LOG_ERR(_T("Connection can't be closed, ioref_=%d"), pConnection->iorefs_);
		return;
	}

	// different activity in server and client side
	if (pConnection->acceptor_)
	{
		InterlockedPushEntrySList(pConnection->acceptor_->free_connection_, pConnection);
	}
	else
	{
		Delete(pConnection);
	}

	SN_LOG_DBG(_T("Close connection success"));
}

void Connection::Delete(Connection* pConnection)
{
	closesocket(pConnection->socket_);
	_aligned_free(pConnection->context_);
	_aligned_free(pConnection);

	SN_LOG_DBG(_T("Close socket success"));
}
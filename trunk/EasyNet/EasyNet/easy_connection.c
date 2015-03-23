#include "easy_connection.h"
#include "easy_worker.h"
#include "easy_acceptor.h"
#include "easy_net.h"
#include "easy_context.h"
#include "easy_log.h"

#ifdef __cplusplus
extern "C" {
#endif

EasyConnection* CreateConnection(EasyHandler* pHandler, EasyWorker* pWorker, EasyAcceptor* pAcceptor)
{
	EasyConnection* pConnection = (EasyConnection*)_aligned_malloc(sizeof(EasyConnection), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnection)
	{
		// initialize connection's tcp socket
		pConnection->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		_ASSERT(pConnection->socket_ != INVALID_SOCKET);
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

				pConnection->context_ = (EasyContext*)_aligned_malloc(sizeof(EasyContext), MEMORY_ALLOCATION_ALIGNMENT);
				_ASSERT(pConnection->context_);
				if (pConnection->context_)
				{
					pConnection->handler_ = *pHandler;
					pConnection->worker_ = pWorker;
					pConnection->acceptor_ = pAcceptor;
					pConnection->context_->connection_ = pConnection;
					pConnection->local_sockaddr_ = NULL;
					pConnection->remote_sockaddr_ = NULL;
					ZeroMemory(&pConnection->context_->overlapped_, sizeof(WSAOVERLAPPED));
					if (!pAcceptor)
					{
						// if acceptor=NULL, means it's called at client side
						// connection's socket must bind to it's address
						SOCKADDR_IN addr;
						ZeroMemory(&addr, sizeof(addr));
						addr.sin_family = AF_INET;
						if (bind(pConnection->socket_, (SOCKADDR*)&addr, sizeof(addr)) == 0)
						{
							LOG_DBG(_T("Create and configure connection in client side"));
							return pConnection;
						}
						else
						{
							_ASSERT( FALSE && _T("Bind error") );
						}
					}
					else
					{
						LOG_DBG(_T("Create and configure connection in server side"));
						return pConnection;
					}
				}
			}
			else
			{
				_ASSERT(FALSE && _T("CreateIoCompletionPort failed"));
				LOG_ERR(_T("CreateIoCompletionPort failed, err=%d"), WSAGetLastError());
				closesocket(pConnection->socket_);
			}
		}

		_aligned_free(pConnection);
	}

	LOG_ERR(_T("Create connection failed, err=%d"), GetLastError());

	return NULL;
}

BOOL ConnectConnection(PSOCKADDR_IN pAddr, EasyHandler* pHandler, EasyWorker* pWorker, void* pClient)
{
	EasyConnection* pConnection = CreateConnection(pHandler, pWorker, NULL);
	_ASSERT(pConnection);
	if (pConnection)
	{
		if (AsyncConnect(pConnection, pAddr, pClient))
		{
			return TRUE;
		}

		CloseConnection(pConnection);
	}

	return FALSE;
}

void CloseConnection(EasyConnection* pConnection)
{
	closesocket(pConnection->socket_);
	_aligned_free(pConnection->context_);
	_aligned_free(pConnection);
	LOG_DBG(_T("Close connection success"));
}

BOOL AsyncConnect(EasyConnection* pConnection, PSOCKADDR_IN addr, void* client)
{
	int32 rc = 0;
	int32 iLastError = 0;
	pConnection->client_ = client;
	// post an asychronous connect
	pConnection->context_->operation_type_ = OPERATION_CONNECT;
	rc = connectex_(pConnection->socket_, (SOCKADDR*)addr, sizeof(*addr), NULL, 0, NULL, &pConnection->context_->overlapped_);
	if (rc == 0)
	{
		iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("ConnectEx failed, err=%d"), iLastError);
			return FALSE;
		}
		else
		{
			LOG_DBG(_T("ConnectEx pending"));
		}
	}

	LOG_DBG(_T("ConnectEx success"));

	return TRUE;
}

void AsyncDisconnect(EasyConnection* pConnection)
{
	int32 rc = 0;
	int32 iLastError = 0;
	
	// post an asychronous disconnect
	pConnection->context_->operation_type_ = OPERATION_DISCONNECT;
	// client can't use TF_REUSE_SOCKET
	rc = disconnectex_(pConnection->socket_, &pConnection->context_->overlapped_, pConnection->acceptor_ ? TF_REUSE_SOCKET: 0, 0);
	if (rc == 0)
	{
		iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("DisconnectEx failed, err=%d"), iLastError);
			return;
		}
		else
		{
			LOG_DBG(_T("DisconnectEx pending"));
		}
	}

	LOG_DBG(_T("DisconnectEx success"));
}

void AsyncSend(EasyConnection* pConnection, EasyContext* pContext)
{
	int32 iLastError = 0;
	DWORD dwXfer;
	pContext->connection_ = pConnection;
	// post an asychronous send
	if (WSASend(pConnection->socket_, &pContext->wsabuf_, 1, &dwXfer, 0, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("WSASend failed, err=%d"), iLastError);
			_aligned_free(pContext);
			AsyncDisconnect(pConnection);
			return;
		}
		else
		{
			LOG_DBG(_T("WSASend Pending"));
		}
	}
}

void AsyncRecv(EasyConnection* pConnection, EasyContext* pContext)
{
	DWORD dwXfer, dwFlag = 0;
	int32 iLastError = 0;
	pContext->connection_ = pConnection;
	pContext->wsabuf_.len = MAX_BUFFER;
	// post an asychronous receive
	if (WSARecv(pConnection->socket_, &pContext->wsabuf_, 1, &dwXfer, &dwFlag, &pContext->overlapped_, NULL) == SOCKET_ERROR)
	{
		iLastError = WSAGetLastError();
		_ASSERT(iLastError == ERROR_IO_PENDING);
		if (iLastError != ERROR_IO_PENDING)
		{
			LOG_ERR(_T("WSARecv failed, err=%d"), iLastError);
			_aligned_free(pContext);
			AsyncDisconnect(pConnection);
			return;
		}
		else
		{
			LOG_DBG(_T("WSARecv Pending"));
		}
	}
}

void AsyncRawSend(EasyConnection* pConnection, uint32 len, char* buf)
{
	EasyContext* pContext = (EasyContext*)((char*)buf - BUFOFFSET);
	_ASSERT(pContext->operation_type_ == OPERATION_SEND);
	if (pContext->operation_type_ != OPERATION_SEND)
	{
		LOG_ERR(_T("Operation type exception, type=%d"), pContext->operation_type_);
		return;
	}

	pContext->wsabuf_.len = len;
	AsyncSend(pConnection, pContext);
}


#ifdef __cplusplus
}
#endif

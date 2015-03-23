#include "easy_worker.h"
#include "easy_connection.h"
#include "easy_context.h"
#include <process.h>
#include "easy_acceptor.h"

#ifdef __cplusplus
extern "C" {
#endif


// todo: pContext may leak except "Accept"
uint32 WINAPI EasyWorkerThread(PVOID pParam)
{
	BOOL bResult;
	DWORD dwNumRead;
	LPOVERLAPPED lpOverlapped = NULL;
	EasyContext* pContext = NULL;
	ULONG_PTR key;
	EasyConnection* pConnection = NULL;
	EasyAcceptor* pAcceptor = NULL;

	EasyWorker* pWorker = (EasyWorker*)pParam;

	do
	{
		// get io response from iocp
		bResult = GetQueuedCompletionStatus(pWorker->iocp_, &dwNumRead, &key, &lpOverlapped, INFINITE);
		if (lpOverlapped)
		{
			pContext = (EasyContext*)((char*)lpOverlapped - CTXOFFSET);
			pConnection = pContext->connection_;
			switch(pContext->operation_type_)
			{
			case OPERATION_ACCEPT:
				{
					pAcceptor = pConnection->acceptor_;
					if (bResult)
					{
						int32 rc = 0;
						// post another accept request
						AcceptConnection(pAcceptor);
						rc = setsockopt(pConnection->socket_, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*)&pAcceptor->socket_, sizeof(pAcceptor->socket_));
						_ASSERT(rc == 0);
						if (rc == 0)
						{
							// confirm connected and invoke handler
							pConnection->connected_ = 1;
							if (pConnection->handler_.OnConnection((ConnID)pConnection))
							{
								// post a receive request
								LOG_DBG(_T("Post a WSARecv after accept"));
								AsyncRecv(pConnection, CreateContext(OPERATION_RECV));
							}
							else
							{
								// post a disconnect request
								LOG_ERR(_T("OnAccept failed"));
								AsyncDisconnect(pConnection);
							}
						}
					}
				}
				break;

			case OPERATION_CONNECT:
				{
					if (bResult)
					{
						int32 rc = 0;
						
						pConnection->connected_ = 1;
						rc = setsockopt(pConnection->socket_, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
						_ASSERT(rc == 0);
						if (rc == 0)
						{
							if (pConnection->handler_.OnConnection((ConnID)pConnection))
							{
								// post a receive request
								LOG_DBG(_T("Post a WSARecv after connect"));
								AsyncRecv(pConnection, CreateContext(OPERATION_RECV));
							}
							else
							{
								// post a disconnect request
								LOG_ERR(_T("OnConnect failed"));
								AsyncDisconnect(pConnection);
							}
						}
					}
					else
					{
						// invoke when connect failed
						LOG_ERR(_T("OnConnectFailed"));
						pConnection->handler_.OnConnectFailed(pConnection->client_);
						CloseConnection(pConnection);
					}
				}
				break;

			case OPERATION_DISCONNECT:
				{
					pConnection->handler_.OnDisconnect((ConnID)pConnection);
				}
				break;

			case OPERATION_RECV:
				{
					if (dwNumRead == 0)
					{
						// post a disconnect request
						LOG_DBG(_T("Client post a disconnect request"));
						_aligned_free(pContext);
						AsyncDisconnect(pConnection);
					}
					else
					{
						pConnection->handler_.OnData((ConnID)pConnection, (uint32)dwNumRead, pContext->buffer_);
						LOG_DBG(_T("Post a WSARecv after recv"));
						AsyncRecv(pConnection, pContext);
					}
				}
				break;

			case OPERATION_SEND:
				{
					_aligned_free(pContext);
				}
				break;
			default:
				break;
			}
		}
		else
		{
			break;
		}
	}while(TRUE);

	InterlockedDecrement((LONG*)&pWorker->thread_count_);
	return 0;
}

EasyWorker* CreateWorker(uint32 iCount)
{
	EasyWorker* pWorker = (EasyWorker*)_aligned_malloc(sizeof(EasyWorker), MEMORY_ALLOCATION_ALIGNMENT);
	if (pWorker)
	{
		pWorker->thread_count_ = 0;
		// create iocp handle
		pWorker->iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		// create all thread
		while (pWorker->thread_count_ < iCount)
		{
			HANDLE hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &EasyWorkerThread, pWorker, 0, NULL);
			CloseHandle(hWorkerThread);

			++pWorker->thread_count_;
		}

		LOG_STT(_T("Initialize worker success, thread count=%d"), pWorker->thread_count_);
	}

	return pWorker;
}

void DestroyWorker(EasyWorker* pWorker)
{
	// if threads are not all closed, wait for them
	while (pWorker->thread_count_)
	{
		PostQueuedCompletionStatus(pWorker->iocp_, 0, 0, NULL);
		Sleep(100);
	}

	LOG_STT(_T("destroy worker success"));

	if (pWorker->iocp_)
	{
		CloseHandle(pWorker->iocp_);
	}

	LOG_STT(_T("destroy iocp handle"));
	_aligned_free(pWorker);
}

#ifdef __cplusplus
}
#endif

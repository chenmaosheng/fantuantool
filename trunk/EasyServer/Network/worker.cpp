#include "worker.h"
#include "connection.h"
#include "context.h"
#include <process.h>
#include "acceptor.h"

void Worker::Init(uint32 iThreadCount)
{
	thread_count_ = 0;
	// create iocp handle
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	// create all thread
	while (thread_count_ < iThreadCount)
	{
		HANDLE hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &Worker::WorkerThread, this, 0, NULL);
		CloseHandle(hWorkerThread);

		++thread_count_;
	}

	LOG_STT(_T("Initialize worker success, thread count=%d"), thread_count_);
}

void Worker::Destroy()
{
	// if threads are not all closed, wait for them
	while (thread_count_)
	{
		PostQueuedCompletionStatus(iocp_, 0, 0, NULL);
		Sleep(100);
	}

	LOG_STT(_T("destroy worker success"));

	if (iocp_)
	{
		CloseHandle(iocp_);
	}

	LOG_STT(_T("destroy iocp handle"));
}

Worker* Worker::CreateWorker(uint32 iCount)
{
	Worker* pWorker = (Worker*)_aligned_malloc(sizeof(Worker), MEMORY_ALLOCATION_ALIGNMENT);
	if (pWorker)
	{
		pWorker->Init(iCount);
	}

	return pWorker;
}

void Worker::DestroyWorker(Worker* pWorker)
{
	pWorker->Destroy();
	_aligned_free(pWorker);
}

// todo: pContext may leak except "Accept"
uint32 WINAPI Worker::WorkerThread(PVOID pParam)
{
	BOOL bResult;
	DWORD dwNumRead;
	LPOVERLAPPED lpOverlapped = NULL;
	Context* pContext = NULL;
	ULONG_PTR key;
	Connection* pConnection = NULL;
	Acceptor* pAcceptor = NULL;

	Worker* pWorker = (Worker*)pParam;

	do
	{
		// get io response from iocp
		bResult = GetQueuedCompletionStatus(pWorker->iocp_, &dwNumRead, &key, &lpOverlapped, INFINITE);
		if (lpOverlapped)
		{
			pContext = (Context*)((char*)lpOverlapped - CTXOFFSET);
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
						pAcceptor->Accept();
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
								pConnection->AsyncRecv(Context::CreateContext(OPERATION_RECV));
							}
							else
							{
								// post a disconnect request
								LOG_ERR(_T("OnAccept failed"));
								pConnection->AsyncDisconnect();
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
								pConnection->AsyncRecv(Context::CreateContext(OPERATION_RECV));
							}
							else
							{
								// post a disconnect request
								LOG_ERR(_T("OnConnect failed"));
								pConnection->AsyncDisconnect();
							}
						}
					}
					else
					{
						// invoke when connect failed
						LOG_ERR(_T("OnConnectFailed"));
						pConnection->handler_.OnConnectFailed(pConnection->client_);
						Connection::Close(pConnection);
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
						pConnection->AsyncDisconnect();
					}
					else
					{
						pConnection->handler_.OnData((ConnID)pConnection, (uint32)dwNumRead, pContext->buffer_);
						LOG_DBG(_T("Post a WSARecv after recv"));
						pConnection->AsyncRecv(pContext);
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
	}while(true);

	InterlockedDecrement((LONG*)&pWorker->thread_count_);
	return 0;
}


#include "worker.h"
#include "connection.h"
#include "context.h"
#include <process.h>
#include "acceptor.h"
#include "context_pool.h"

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

	SN_LOG_STT(_T("Initialize worker success, thread count=%d"), thread_count_);
}

void Worker::Destroy()
{
	// if threads are not all closed, wait for them
	while (thread_count_)
	{
		PostQueuedCompletionStatus(iocp_, 0, 0, NULL);
		Sleep(100);
	}

	SN_LOG_STT(_T("destroy worker success"));

	if (iocp_)
	{
		CloseHandle(iocp_);
	}

	SN_LOG_STT(_T("destroy iocp handle"));
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
							if (pConnection->handler_.OnConnection((ConnID)pConnection) && 
								(pContext = pConnection->context_pool_->PopInputContext()))
							{
								// post a receive request
								SN_LOG_DBG(_T("Post a WSARecv after accept"));
								pConnection->AsyncRecv(pContext);
							}
							else
							{
								// post a disconnect request
								SN_LOG_ERR(_T("OnAccept failed"));
								pConnection->AsyncDisconnect();
							}
						}

						InterlockedDecrement(&pAcceptor->iorefs_);
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
							if (pConnection->handler_.OnConnection((ConnID)pConnection) &&
								(pContext = pConnection->context_pool_->PopInputContext()))
							{
								// post a receive request
								SN_LOG_DBG(_T("Post a WSARecv after connect"));
								pConnection->AsyncRecv(pContext);
							}
							else
							{
								// post a disconnect request
								SN_LOG_ERR(_T("OnConnect failed"));
								pConnection->AsyncDisconnect();
							}
						}
					}
					else
					{
						// invoke when connect failed
						SN_LOG_ERR(_T("OnConnectFailed"));
						pConnection->handler_.OnConnectFailed(pConnection->client_);
						Connection::Close(pConnection);
					}
				}
				break;

			case OPERATION_DISCONNECT:
				{
					SN_LOG_DBG(_T("OnDisconnect, iorefs=%d"), pConnection->iorefs_);
					if (pConnection->iorefs_)
					{
						PostQueuedCompletionStatus(pWorker->iocp_, dwNumRead, key, lpOverlapped);
					}
					else
					{
						pConnection->handler_.OnDisconnect((ConnID)pConnection);
					}
				}
				break;

			case OPERATION_RECV:
				{
					if (dwNumRead == 0)
					{
						// post a disconnect request
						SN_LOG_DBG(_T("Client post a disconnect request"));
						pConnection->context_pool_->PushInputContext(pContext);
						pConnection->AsyncDisconnect();
					}
					else
					{
						pConnection->handler_.OnData((ConnID)pConnection, (uint32)dwNumRead, pContext->buffer_);
						SN_LOG_DBG(_T("Post a WSARecv after recv"));
						pConnection->AsyncRecv(pContext);
					}
					
					InterlockedDecrement(&pConnection->iorefs_);
				}
				break;

			case OPERATION_SEND:
				{
					pConnection->context_pool_->PushOutputContext(pContext);
					InterlockedDecrement(&pConnection->iorefs_);
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


#include "worker.h"
#include "connection.h"
#include "context.h"
#include <process.h>
#include "acceptor.h"
#include "context_pool.h"

Worker::Worker()
{
}

Worker::~Worker()
{
}

void Worker::Init()
{
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	HANDLE hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &Worker::WorkerThread, this, 0, NULL);
	CloseHandle(hWorkerThread);
}

void Worker::Destroy()
{
	PostQueuedCompletionStatus(iocp_, 0, 0, NULL);
	if (iocp_)
	{
		CloseHandle(iocp_);
	}
}

Worker* Worker::CreateWorker()
{
	Worker* pWorker = (Worker*)_aligned_malloc(sizeof(Worker), MEMORY_ALLOCATION_ALIGNMENT);
	if (pWorker)
	{
		pWorker->Init();
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
						pAcceptor->Accept();
						rc = setsockopt(pConnection->socket_, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*)&pAcceptor->socket_, sizeof(pAcceptor->socket_));
						if (rc == 0)
						{
							pConnection->connected_ = 1;
							if (pConnection->handler_.OnConnection((ConnID)pConnection) && 
								(pContext = pConnection->context_pool_->PopInputContext()))
							{
								pConnection->AsyncRecv(pContext);
							}
							else
							{
								pConnection->Disconnect();
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
						if (rc == 0)
						{
							if (pConnection->handler_.OnConnection((ConnID)pConnection) &&
								(pContext = pConnection->context_pool_->PopInputContext()))
							{
								pConnection->AsyncRecv(pContext);
							}
							else
							{
								pConnection->Disconnect();
							}
						}
					}
					else
					{
						pConnection->handler_.OnConnectFailed(pConnection->client_);
						Connection::Close(pConnection);
					}
				}
				break;

			case OPERATION_DISCONNECT:
				{
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
						printf("client %s disconnected\n", inet_ntoa(pConnection->sockaddr_.sin_addr));
						pConnection->context_pool_->PushInputContext(pContext);
						pConnection->Disconnect();
					}
					else
					{
						printf("client %s send something\n", inet_ntoa(pConnection->sockaddr_.sin_addr));
						
						pConnection->handler_.OnData((ConnID)pConnection, (uint16)dwNumRead, pContext->buffer_);
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

	return 0;
}


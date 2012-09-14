#include "worker.h"
#include "connection.h"
#include "context.h"
#include <process.h>

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

uint32 WINAPI Worker::WorkerThread(PVOID pParam)
{
	BOOL bResult;
	DWORD dwNumRead;
	Connection* pConnection = NULL;
	LPOVERLAPPED lpOverlapped = NULL;
	Context* pContext = NULL;

	Worker* pWorker = (Worker*)pParam;

	while (true)
	{
		bResult = GetQueuedCompletionStatus(pWorker->iocp_, &dwNumRead, (ULONG_PTR*)&pConnection, &lpOverlapped, INFINITE);
		if (lpOverlapped)
		{
			pContext = (Context*)((char*)lpOverlapped - CTXOFFSET);
			switch(pContext->operation_type_)
			{
			case OPERATION_RECV:
				{
					if (dwNumRead == 0)
					{
						printf("client %s disconnected\n", inet_ntoa(pConnection->sockAddr_.sin_addr));
						pConnection->handler_.OnDisconnect((ConnID)pConnection);
					}
					else
					{
						printf("client %s send something\n", inet_ntoa(pConnection->sockAddr_.sin_addr));
						
						pConnection->handler_.OnData((ConnID)pConnection, (uint16)dwNumRead, pContext->buffer_);
						pConnection->AsyncRecv();
					}
					
				}
				break;

			case OPERATION_SEND:
				{
					
				}
				break;
			default:
				break;
			}
		}

	}
}


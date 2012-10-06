#include "worker.h"
#include "connector.h"
#include <process.h>

void Worker::Init()
{
	m_pConnector = NULL;
	HANDLE hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &Worker::WorkerThread, this, 0, NULL);
	CloseHandle(hWorkerThread);
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
	_aligned_free(pWorker);
}

uint32 WINAPI Worker::WorkerThread(PVOID pParam)
{
	Worker* pWorker = (Worker*)pParam;
	
	while (true)
	{
		if (pWorker->m_pConnector && pWorker->m_pConnector->IsConnected())
		{
			pWorker->m_pConnector->Recv();
		}
	}

	return 0;
}

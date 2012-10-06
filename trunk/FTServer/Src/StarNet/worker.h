/*****************************************************************************************
	filename:	worker.h
	created:	09/27/2012
	author:		chen
	purpose:	thread for handle iocp response

*****************************************************************************************/

#ifndef _H_WORKER
#define _H_WORKER

#include "starnet_common.h"

class Worker
{
public:
	// initialize worker with several threads
	void Init(uint32 iThreadCount);
	// destroy worker
	void Destroy();

	static Worker* CreateWorker(uint32 iThreadCount);
	static void DestroyWorker(Worker*);

	static uint32 WINAPI WorkerThread(PVOID);

public:
	HANDLE	iocp_;			// binded iocp handle
	uint32	thread_count_;	// the number of threads
};

#endif

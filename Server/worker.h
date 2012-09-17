#ifndef _H_WORKER
#define _H_WORKER

#include "common.h"
#include "connection.h"
#include <vector>

class Worker
{
public:
	void Init(uint32 iCount);
	void Destroy();

	static Worker* CreateWorker(uint32 iCount);
	static void DestroyWorker(Worker*);

	static uint32 WINAPI WorkerThread(PVOID);

public:
	HANDLE	iocp_;
	uint32	thread_count_;
};

#endif

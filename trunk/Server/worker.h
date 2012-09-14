#ifndef _H_WORKER
#define _H_WORKER

#include "common.h"
#include "connection.h"
#include <vector>

class Worker
{
public:
	Worker();
	~Worker();

	void Init();
	void Destroy();

	static uint32 WINAPI WorkerThread(PVOID);

public:
	HANDLE	iocp_;
};

#endif

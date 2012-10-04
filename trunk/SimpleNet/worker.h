/*****************************************************************************************
	filename:	worker.h
	created:	10/04/2012
	author:		chen
	purpose:	thread for handle winsock response

*****************************************************************************************/

#ifndef _H_WORKER
#define _H_WORKER

#include "simplenet_common.h"

struct Connector;
class Worker
{
public:
	// initialize worker
	void Init();
	
	static Worker* CreateWorker();
	static void DestroyWorker(Worker*);

	static uint32 WINAPI WorkerThread(PVOID);

public:
	Connector* m_pConnector;
};

#endif

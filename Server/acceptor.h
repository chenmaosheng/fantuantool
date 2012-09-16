#ifndef _H_ACCEPTOR
#define _H_ACCEPTOR

#include "handler.h"

class Worker;
class ContextPool;
struct Context;
class Acceptor
{
public:
	void Init(PSOCKADDR_IN addr, Worker* pWorker, ContextPool* pContextPool, Handler* pHandler);
	void Destroy();
	void Accept();

	void Start();
	void Stop();

	static Acceptor* CreateAcceptor(PSOCKADDR_IN addr, Worker* pWorker, ContextPool* pContextPool, Handler* pHandler);
	static void DestroyAcceptor(Acceptor*);

public:
	SOCKET	socket_;
	Handler	handler_;
	Worker*	worker_;
	ContextPool* context_pool_;
	void*	server_;			// related server
	Context* context_;
	
	LONG	iorefs_;
	LONG	running_;
	uint32	total_connection_;
	PSLIST_HEADER free_connection_;

};

#endif

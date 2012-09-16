#ifndef _H_ACCEPTOR
#define _H_ACCEPTOR

#include "handler.h"

class Worker;
class ContextPool;
class Acceptor
{
public:
	Acceptor(){}
	~Acceptor(){}

	void Init(PSOCKADDR_IN addr, Worker* pWorker, ContextPool* pContextPool, Handler* pHandler);
	void Destroy();
	void Accept();

	void Start();

	static Acceptor* CreateAcceptor(PSOCKADDR_IN addr, Worker* pWorker, ContextPool* pContextPool, Handler* pHandler);
	static void DestroyAcceptor(Acceptor*);

public:
	SOCKET	socket_;
	Handler	handler_;
	Worker*	worker_;
	ContextPool* context_pool_;
};

#endif

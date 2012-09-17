#ifndef _H_SERVER_BASE
#define _H_SERVER_BASE

#include "common.h"

class Worker;
class Acceptor;
class ContextPool;
struct Handler;
class ServerBase
{
public:
	virtual int32 Init();
	virtual void Destroy();

	ContextPool* GetContextPool();
	
protected:
	ServerBase();
	virtual ~ServerBase();

	int32 InitAcceptor(uint32 ip, uint16 port, Handler* pHandler, uint32 iThreadCount);
	void DestroyAcceptor();

	void StartAcceptor();
	void StopAcceptor();

protected:
	Acceptor* acceptor_;
	Worker* worker_;
	ContextPool* context_pool_;
};

#endif

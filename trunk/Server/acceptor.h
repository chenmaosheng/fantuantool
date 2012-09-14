#ifndef _H_ACCEPTOR
#define _H_ACCEPTOR

#include "handler.h"

class Worker;
class Acceptor
{
public:
	Acceptor();
	~Acceptor();

	void Init(PSOCKADDR_IN addr, Worker* pWorker, Handler* pHandler);
	void Destroy();
	void Accept();

	void Start();

public:
	SOCKET	socket_;
	Handler	handler_;
	Worker*	worker_;
};

#endif

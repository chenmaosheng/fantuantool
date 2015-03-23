#ifndef _H_ACCEPTOR
#define _H_ACCEPTOR

#include "handler.h"
#include "context.h"

class Worker;
struct Context;
class Acceptor
{
public:
	// initialize the acceptor, but not running at first
	int32 Init(PSOCKADDR_IN addr, Worker* pWorker, Handler* pHandler);
	// stop and destroy the acceptor, close all connection
	void Destroy();

	// start the acceptor and ready to receive connection
	void Start();
	// stop receiving connection
	void Stop();
	// post asynchronous accept to receive oncoming connection
	void Accept();

	// set the bind server from app layer
	void SetServer(void*);
	void* GetServer();

	// use these two static functions to create and destroy acceptor
	static Acceptor* CreateAcceptor(PSOCKADDR_IN addr, Worker* pWorker, Handler* pHandler);
	static void DestroyAcceptor(Acceptor*);

public:
	SOCKET  socket_;
	Handler handler_;                               // io handler
	Worker* worker_;                                // worker thread
	void*   server_;                                // related server
	Context context_;                               // initial context
        
	LONG    running_;                               // is running or not
};

#endif

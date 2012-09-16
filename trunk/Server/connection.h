#ifndef _H_CONNECTION
#define _H_CONNECTION

#include "handler.h"

typedef HANDLE	ConnID;

struct Context;
class ContextPool;
class Worker;
class Acceptor;
struct Connection : SLIST_ENTRY
{
	SOCKET			socket_;
	SOCKADDR_IN		sockaddr_;
	Handler			handler_;
	ContextPool*	context_pool_;
	Worker*			worker_;
	Acceptor*		acceptor_;
	Context*		context_;
	void*			client_;
	
	LONG			iorefs_;
	LONG			connected_;
	LONG			iorefmax_;
	
	int32 Connect(PSOCKADDR_IN addr, void* client);
	void Disconnect();
	void AsyncSend(Context*);
	void AsyncRecv(Context*);

	static Connection* Create(Handler* pHandler, ContextPool* pContextPool, Worker* pWorker, Acceptor* pAcceptor);
	static void Close(Connection*);
};

#endif

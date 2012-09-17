#ifndef _H_CONNECTION
#define _H_CONNECTION

#include "handler.h"

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
	
	int32 AsyncConnect(PSOCKADDR_IN addr, void* client);
	void AsyncDisconnect();
	void AsyncSend(Context*);
	void AsyncRecv(Context*);
	void AsyncSend(uint16 len, char* buf);

	void SetClient(void*);
	void* GetClient();
	void SetRefMax(uint16 iMax);
	bool IsConnected();

	static Connection* Create(Handler* pHandler, ContextPool* pContextPool, Worker* pWorker, Acceptor* pAcceptor);
	static void Close(Connection*);
	static void Delete(Connection*);
};

#endif

/*****************************************************************************************
	filename:	connection.h
	created:	09/27/2012
	author:		chen
	purpose:	create a connection to restore connect from client

*****************************************************************************************/

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
	SOCKADDR_IN		sockaddr_;		// connetion's address
	Handler			handler_;		// io handler
	ContextPool*	context_pool_;	// point to context pool
	Worker*			worker_;
	Acceptor*		acceptor_;		// related acceptor
	Context*		context_;		// initial context
	void*			client_;		// pointer from app layer
	
	LONG			iorefs_;		// io reference counter
	LONG			connected_;		// is connected
	LONG			iorefmax_;		// max io reference allowed
	
	// asynchronous connect
	bool AsyncConnect(PSOCKADDR_IN addr, void* client);
	// asynchronous disconnect
	void AsyncDisconnect();
	// asynchronous send, need pop a context first
	void AsyncSend(Context*);
	// asynchronous receive, need pop a context first
	void AsyncRecv(Context*);
	void AsyncSend(uint32 len, char* buf);

	void SetClient(void*);
	void* GetClient();
	void SetRefMax(uint16 iMax);
	bool IsConnected();

	// static function to create and close
	static Connection* Create(Handler* pHandler, ContextPool* pContextPool, Worker* pWorker, Acceptor* pAcceptor);
	static bool Connect(PSOCKADDR_IN pAddr, Handler* pHandler, ContextPool* pContextPool, Worker* pWorker, void* pClient);
	static void Close(Connection*);	// attention: don't call this function if disconnect not called
	static void Delete(Connection*);
};

#endif

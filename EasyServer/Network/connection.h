#ifndef _H_CONNECTION
#define _H_CONNECTION

#include "handler.h"

struct Context;
class Worker;
class Acceptor;
struct Connection
{
	SOCKET                  socket_;        
	SOCKADDR_IN             sockaddr_;              // connetion's address
	Handler                 handler_;               // io handler
	Worker*                 worker_;
	Acceptor*               acceptor_;              // related acceptor
	Context*                context_;               // initial context
	void*                   client_;                // pointer from app layer    
	LONG                    connected_;             // is connected
        
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
	bool IsConnected();

	// static function to create and close
	static Connection* Create(Handler* pHandler, Worker* pWorker, Acceptor* pAcceptor);
	static bool Connect(PSOCKADDR_IN pAddr, Handler* pHandler, Worker* pWorker, void* pClient);
	static void Close(Connection*); // attention: don't call this function if disconnect not called
	static void Delete(Connection*);
};

#endif

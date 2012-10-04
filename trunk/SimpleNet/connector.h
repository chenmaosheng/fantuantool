/*****************************************************************************************
	filename:	connector.h
	created:	10/04/2012
	author:		chen
	purpose:	create a connection to restore connect info

*****************************************************************************************/

#ifndef _H_CONNECTOR
#define _H_CONNECTOR

#include "handler.h"

class Worker;
struct Connector
{
	SOCKET			socket_;	
	SOCKADDR_IN		sockaddr_;		// connection's address
	Handler			handler_;		// io handler
	Worker*			worker_;
	void*			client_;		// pointer from app layer

	LONG			connected_;		// is connected
	
	// synchronous connect
	bool Connect(PSOCKADDR_IN addr, void* client);
	// synchronous disconnect
	void Disconnect();
	// synchronous receive
	void Recv();
	void Send(int32 iLen, char* pBuf);

	void SetClient(void*);
	void* GetClient();
	bool IsConnected();

	// static function to create and close
	static Connector* Create(Handler* pHandler, Worker* pWorker);
	static bool Connect(PSOCKADDR_IN pAddr, Handler* pHandler, Worker* pWorker, void* pClient);
	static void Close(Connector*);
	static void Delete(Connector*);
};

#endif

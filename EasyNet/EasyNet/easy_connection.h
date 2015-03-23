#ifndef _H_EASY_CONNECTION
#define _H_EASY_CONNECTION

#include "easy_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _EasyWorker;
struct _EasyAcceptor;
struct _EasyContext;
typedef struct _EasyConnection
{
	SOCKET					socket_;        
	SOCKADDR_IN*			local_sockaddr_;		// connetion's address
	SOCKADDR_IN*			remote_sockaddr_;		// remote connection's address
	EasyHandler				handler_;				// io handler
	struct _EasyWorker*		worker_;
	struct _EasyAcceptor*	acceptor_;				// related acceptor
	struct _EasyContext*	context_;				// initial context
	void*					client_;				// pointer from app layer    
}EasyConnection;

// static function to create and close
extern EasyConnection*		CreateConnection(EasyHandler* pHandler, struct _EasyWorker* pWorker, struct _EasyAcceptor* pAcceptor);
extern BOOL					ConnectConnection(PSOCKADDR_IN pAddr, EasyHandler* pHandler, struct _EasyWorker* pWorker, void* pClient);
extern void					CloseConnection(EasyConnection*); // attention: don't call this function if disconnect not called
// asynchronous connect
extern BOOL					AsyncConnect(EasyConnection*, PSOCKADDR_IN addr, void* client);
// asynchronous disconnect
extern void					AsyncDisconnect(EasyConnection*);
// asynchronous send, need pop a context first
extern void					AsyncSend(EasyConnection*, struct _EasyContext*);
// asynchronous receive, need pop a context first
extern void					AsyncRecv(EasyConnection*, struct _EasyContext*);
extern void					AsyncRawSend(EasyConnection*, uint32 len, char* buf);

#ifdef __cplusplus
}
#endif

#endif

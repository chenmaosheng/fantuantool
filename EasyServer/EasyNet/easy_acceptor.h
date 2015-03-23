#ifndef _H_EASY_ACCEPTOR
#define _H_EASY_ACCEPTOR

#include "easy_handler.h"
#include "easy_context.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _EasyWorker;
typedef struct _EasyAcceptor
{
	SOCKET				socket_;
	EasyHandler			handler_;                               // io handler
	void*				server_;                                // related server
	EasyContext			context_;                               // initial context
	LONG				running_;                               // is running or not
	struct _EasyWorker*	worker_;                                // worker thread
}EasyAcceptor;

// initialize the acceptor, but not running at first
extern EasyAcceptor*	CreateAcceptor(PSOCKADDR_IN addr, struct _EasyWorker* pWorker, EasyHandler* pHandler);
// stop and destroy the acceptor, close all connection
extern void				DestroyAcceptor(EasyAcceptor*);
// post asynchronous accept to receive oncoming connection
void					AcceptConnection(EasyAcceptor*);

// set the bind server from app layer
void					SetServerApp(EasyAcceptor*, void*);
void*					GetServerApp(EasyAcceptor*);

#ifdef __cplusplus
}
#endif

#endif

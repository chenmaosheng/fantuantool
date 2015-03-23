#ifndef _H_EASY_ACCEPTOR
#define _H_EASY_ACCEPTOR

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _EasyWorker;
struct _EasyConnection;
typedef struct _EasyAcceptor
{
	SOCKET				socket_;
	void*				server_;                                // related server
}EasyAcceptor;

// initialize the acceptor, but not running at first
extern EasyAcceptor*	CreateAcceptor(uint32 ip, uint16 port);
// stop and destroy the acceptor, close all connection
extern void				DestroyAcceptor(EasyAcceptor*);
// post asynchronous accept to receive oncoming connection
extern struct _EasyConnection*	AcceptConnection(EasyAcceptor*);

#ifdef __cplusplus
}
#endif

#endif

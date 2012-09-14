#ifndef _H_CONNECTION
#define _H_CONNECTION

#include "context.h"
#include "handler.h"

typedef HANDLE	ConnID;

struct Connection : SLIST_ENTRY
{
	SOCKET			socket_;
	SOCKADDR_IN		sockAddr_;
	Context			sendContext_;
	Context			recvContext_;
	LONG			connected_;
	Handler			handler_;
	
	Connection();

	void AsyncRecv();

	void AsyncSend(int len, char* buf);
};

#endif

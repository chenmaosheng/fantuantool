/*****************************************************************************************
	filename:	socket_base.h
	created:	11/10/2012
	author:		chen
	purpose:	basic socket setting for both acceptor and connection

*****************************************************************************************/

#ifndef _H_SOCKET_BASE
#define _H_SOCKET_BASE

#include "uninet_common.h"

struct SocketBase
{
	SOCKET m_Socket;
	SOCKADDR_IN m_SockAddr;
	uint32 m_iMTU;
};

#endif

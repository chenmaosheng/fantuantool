/*****************************************************************************************
	filename:	acceptor.h
	created:	11/10/2012
	author:		chen
	purpose:	create an acceptor and receive connection from client

*****************************************************************************************/

#ifndef _H_ACCEPTOR
#define _H_ACCEPTOR

#include "uninet_common.h"

class Acceptor
{
public:
	// initialize the acceptor, but not running at first
	int32 Init(PSOCKADDR_IN pAddr);
	// stop and destroy the acceptor, close all connection
	void Destroy();

public:
	SOCKET m_Socket;
};

#endif

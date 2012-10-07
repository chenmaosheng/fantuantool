/*****************************************************************************************
	filename:	client_base.h
	created:	10/03/2012
	author:		chen
	purpose:	basic function of each client

*****************************************************************************************/

#ifndef _H_CLIENT_BASE
#define _H_CLIENT_BASE

#include "client_common.h"

class Worker;
class ClientBase
{
public:
	ClientBase();
	~ClientBase();

	// initialize client
	virtual int32 Init();
	// destroy client
	virtual void Destroy();

	void Login(const char* strHost, const char* strToken);

private:
	Worker* m_pWorker;
	Log* m_pLogSystem;
};

#endif

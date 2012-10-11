/*****************************************************************************************
	filename:	ClientLogic.h
	created:	10/11/2012
	author:		chen
	purpose:	separate logic from client ui

*****************************************************************************************/

#ifndef _H_CLIENT_LOGIC
#define _H_CLIENT_LOGIC

#include "client_base.h"

class ClientLogic
{
public:
	ClientLogic(ClientBase* pClientBase);
	~ClientLogic();

	void OnIncomingEvent();

	void Login(const TCHAR* strToken);
	void Logout();

private:
	ClientBase* m_pClientBase;
};

#endif

/*****************************************************************************************
	filename:	login_server_loop.h
	created:	09/27/2012
	author:		chen
	purpose:	control game tick on login server

*****************************************************************************************/

#ifndef _H_LOGIN_SERVER_LOOP
#define _H_LOGIN_SERVER_LOOP

#include "session_server_loop.h"
#include "login_session.h"

class LoginServerLoop : public SessionServerLoop<LoginSession>
{
public:
	typedef SessionServerLoop super;

	LoginServerLoop();
	~LoginServerLoop();

	int32 Init();
	void Destroy();

	int32 Start();

private:
	uint32 _Loop();
	bool _OnCommand(LogicCommand*);
};

#endif

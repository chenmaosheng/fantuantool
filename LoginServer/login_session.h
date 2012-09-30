/*****************************************************************************************
	filename:	login_session.h
	created:	09/27/2012
	author:		chen
	purpose:	inherit from session, only for login server

*****************************************************************************************/

#ifndef _H_LOGIN_SESSION
#define _H_LOGIN_SESSION

#include "session.h"

enum
{
	SESSION_STATE_ONLOGINREQ = SESSION_STATE_USERDEFINED + 1,	// receive login req
	SESSION_STATE_ONLOGINACK, // receive login ack
	SESSION_STATE_ONGATEHOLDNTF, // notify gate hold
	SESSION_STATE_ONVERSIONREQ, // receive version req
};

enum
{
	SESSION_EVENT_ONLOGINREQ = SESSION_EVENT_USERDEFINED + 1,
	SESSION_EVENT_ONLOGINACK,
	SESSION_EVENT_ONGATEHOLDNTF,
	SESSION_EVENT_ONVERSIONREQ,
};

class LoginServerLoop;
class LoginSession : public Session
{
public:
	typedef Session super;

	LoginSession();
	~LoginSession();

	void Clear();		// when reuse, clear the session

	int32 OnConnection(ConnID connId);
	void OnDisconnect();
	void Disconnect();

	// receive login failed ack from master server
	void OnLoginAck(int32 iReturn);
	// receive version check req from client
	void OnVersionReq(uint32 iVersion);

private:
	// initialize state machine
	void InitStateMachine();
	// check token
	int32 CheckLoginToken(uint16 iLen, char* pBuf);
	// notify login success to client
	int32 LoggedInNtf();

public:
	static LoginServerLoop* m_pMainLoop;

private:
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

#endif

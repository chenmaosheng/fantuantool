/*****************************************************************************************
	filename:	gate_session.h
	created:	09/27/2012
	author:		chen
	purpose:	inherit from session, only for gate server

*****************************************************************************************/

#ifndef _H_GATE_SESSION
#define _H_GATE_SESSION

#include "session.h"

enum
{
	SESSION_STATE_TRANSFERED = SESSION_STATE_USERDEFINED + 1, // from loginsession to gatesession
	SESSION_STATE_ONGATEHOLDREQ, // receive gate hold req
	SESSION_STATE_GATEHOLDACK, // send gate hold ack
	SESSION_STATE_GATERELEASEREQ, // receive gate release req
	SESSION_STATE_GATELOGINREQ, // send gate login to master
};

enum
{
	SESSION_EVENT_TRANSFERED = SESSION_EVENT_USERDEFINED + 1,
	SESSION_EVENT_ONGATEHOLDREQ,
	SESSION_EVENT_GATEHOLDACK,
	SESSION_EVENT_GATERELEASEREQ,
	SESSION_EVENT_GATELOGINREQ,
};

class GateServerLoop;
class GateSession : public Session
{
public:
	typedef Session super;

	GateSession();
	~GateSession();
	void Clear();		// when reuse, clear the session

	int32 OnConnection(ConnID connId);
	void OnDisconnect();
	void Disconnect();

	void OnHoldReq(uint32 iLoginSessionId, const TCHAR* strAccountName);

private:
	// initialize state machine
	void InitStateMachine();
	// check token
	int32 CheckLoginToken(uint16 iLen, char* pBuf);
	// notify login success to client
	int32 LoggedInNtf();
	// finish session transfered
	void OnSessionTransfered();

public:
	static GateServerLoop* m_pMainLoop;

private:
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	uint32 m_iLoginSessionId;
	bool m_bTempSession;	// before finished login, the session is allocated as temp session
};

#endif

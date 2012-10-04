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
	SESSION_STATE_ONGATEALLOCREQ, // receive gate allocate req
	SESSION_STATE_GATEALLOCACK, // send gate allocate ack
	SESSION_STATE_GATERELEASEREQ, // receive gate release req
	SESSION_STATE_GATELOGINREQ, // send gate login to master
	SESSION_STATE_ONMASTERDISCONNECT, // receive force disconnect from master server
};

enum
{
	SESSION_EVENT_TRANSFERED = SESSION_EVENT_USERDEFINED + 1,
	SESSION_EVENT_ONGATEALLOCREQ,
	SESSION_EVENT_GATEALLOCACK,
	SESSION_EVENT_GATERELEASEREQ,
	SESSION_EVENT_GATELOGINREQ,
	SESSION_EVENT_ONMASTERDISCONNECT,
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
	// receive disconnect from master
	void OnMasterDisconnect();
	void OnGateAllocReq(uint32 iLoginSessionId, const TCHAR* strAccountName);
	void OnGateReleaseReq();
	// clone session from source
	void Clone(GateSession* pSession);
	// finish session transfered
	void OnSessionTransfered();

private:
	// initialize state machine
	void InitStateMachine();
	// check token
	int32 CheckLoginToken(uint16 iLen, char* pBuf);
	// notify login success to client
	int32 LoggedInNtf();

public:
	static GateServerLoop* m_pMainLoop;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	uint32 m_iLoginSessionId;
	bool m_bFinalizing;

private:
	bool m_bTempSession;	// before finished login, the session is allocated as temp session
};

#endif

#ifndef _H_GATE_SESSION
#define _H_GATE_SESSION

#include "session.h"

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

public:
	static GateServerLoop* m_pMainLoop;

private:
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

#endif

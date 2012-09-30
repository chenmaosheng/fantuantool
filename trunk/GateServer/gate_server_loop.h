/*****************************************************************************************
	filename:	gate_server_loop.h
	created:	09/27/2012
	author:		chen
	purpose:	control game tick on gate server

*****************************************************************************************/

#ifndef _H_GATE_SERVER_LOOP
#define _H_GATE_SERVER_LOOP

#include "session_server_loop.h"
#include "gate_session.h"
#include <hash_map>
#include <string>

struct LogicCommandGateHoldReq;
class GateSession;
class GateServerLoop : public SessionServerLoop<GateSession>
{
public:
	typedef SessionServerLoop super;

	GateServerLoop();
	~GateServerLoop();

	int32 Init();
	void Destroy();

	// start gate server's loop
	int32 Start();

	// temp session to session
	int32 TransferSession(uint32 iTempSessionId, TCHAR* strAccountName, GateSession*& pSession);
	// close session
	void CloseSession(GateSession*);

private:
	DWORD _Loop();
	bool _OnCommand(LogicCommand*);

private:
	void _OnCommandGateHoldReq(LogicCommandGateHoldReq*);

private:
	stdext::hash_map<std::wstring, GateSession*> m_mSessionMapByName;
};

#endif

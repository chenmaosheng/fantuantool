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
#include <queue>

struct LogicCommandGateAllocReq;
struct LogicCommandDisconnect;
struct LogicCommandGateReleaseReq;
struct LogicCommandRegionBindReq;
struct LogicCommandBroadcastData;
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
	int32 TransferSession(uint32 iTempSessionId, TCHAR* strAccountName, GateSession*& pOutputSession);
	// close session
	void CloseSession(GateSession*, bool isByMaster = false);
	// clear session
	void ClearSession(GateSession*);
	
private:
	DWORD _Loop();
	bool _OnCommand(LogicCommand*);

private:
	void _OnCommandOnConnect(LogicCommandOnConnect*);
	void _OnCommandOnData(LogicCommandOnData*);
	void _OnCommandOnDisconnect(LogicCommandOnDisconnect*);
	void _OnCommandDisconnect(LogicCommandDisconnect*);
	void _OnCommandSendData(LogicCommandSendData*);
	void _OnCommandBroadcastData(LogicCommandBroadcastData*);
	void _OnCommandShutdown();
	
	void _OnCommandGateAllocReq(LogicCommandGateAllocReq*);
	void _OnCommandGateReleaseReq(LogicCommandGateReleaseReq*);
	void _OnCommandRegionBindReq(LogicCommandRegionBindReq*);

	// check if any session disconnect, it's a heart beat test
	void _CheckSessionState();
	void _ReportState();

private:
	stdext::hash_map<std::wstring, GateSession*> m_mSessionMapByName;
	std::queue<GateSession*> m_SessionFinalizingQueue;

	uint16 m_iSessionMax;
	uint16 m_iTempSessionMax;
	uint16 m_iSessionCount;
	uint16 m_iTempSessionCount;
};

#endif

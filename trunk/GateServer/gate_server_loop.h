#ifndef _H_GATE_SERVER_LOOP
#define _H_GATE_SERVER_LOOP

#include "session_server_loop.h"
#include "gate_session.h"
#include <hash_map>

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

	int32 Start();

private:
	uint32 _Loop();
	bool _OnCommand(LogicCommand*);

private:
	void _OnCommandGateHoldReq(LogicCommandGateHoldReq*);

private:
	stdext::hash_map<std::wstring, GateSession*> m_mSessionMapByName;
};

#endif

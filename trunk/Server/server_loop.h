#ifndef _H_SERVER_LOOP
#define _H_SERVER_LOOP

#include "logic_loop.h"
#include <map>

class ServerBase;
class Session;
class LogicCommand;
class LogicCommandOnConnect;
class LogicCommandOnDisconnect;
class LogicCommandOnData;
class LogicCommandBroadcastData;
class ServerLoop : public LogicLoop
{
public:
	ServerLoop();
	~ServerLoop();

	int32 Init();
	void Destroy();

	Session* GetSession(uint32 iSessionId);

private:
	virtual DWORD _Loop();
	virtual bool _OnCommand(LogicCommand*);
	void _OnCommandOnConnect(LogicCommandOnConnect*);
	void _OnCommandOnDisconnect(LogicCommandOnDisconnect*);
	void _OnCommandOnData(LogicCommandOnData*);
	void _OnCommandBroadcastData(LogicCommandBroadcastData*);

private:
	std::map<uint32, Session*> m_mSessionList;
	uint8 m_iServerId;

};
#endif

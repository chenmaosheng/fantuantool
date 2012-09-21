#ifndef _H_SERVER_LOOP
#define _H_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include <hash_map>

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
	ServerLoop(uint16 iSessionMax = 32768);
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
	stdext::hash_map<uint32, Session*> m_mSessionMap;
	ObjectPool<Session> m_SessionPool;
	Session** m_arraySession;
	uint16 m_iSessionMax;
};
#endif

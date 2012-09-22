#ifndef _H_SESSION_SERVER_LOOP
#define _H_SESSION_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include <hash_map>

class ServerBase;
class Session;
struct LogicCommand;
struct LogicCommandOnConnect;
struct LogicCommandOnDisconnect;
struct LogicCommandOnData;
struct LogicCommandBroadcastData;
template<typename T>
class SessionServerLoop : public LogicLoop
{
public:
	enum { NOT_SHUTDOWN, START_SHUTDOWN, READY_FOR_SHUTDOWN, };

	SessionServerLoop(uint16 iSessionMax = 32768);
	~SessionServerLoop();

	int32 Init(ServerBase* pServer);
	void Destroy();

	bool IsReadyForShutdown() const;

protected:
	T* GetSession(uint32 iSessionId);
	void _ReadyForShutdown();
	virtual bool _OnCommand(LogicCommand*);

private:
	void _OnCommandOnConnect(LogicCommandOnConnect*);
	void _OnCommandOnDisconnect(LogicCommandOnDisconnect*);
	void _OnCommandOnData(LogicCommandOnData*);
	void _OnCommandBroadcastData(LogicCommandBroadcastData*);
	void _OnCommandShutdown();	

private:
	stdext::hash_map<uint32, T*> m_mSessionMap;
	ObjectPool<T> m_SessionPool;
	T** m_arraySession;
	uint16 m_iSessionMax;

protected:
	int32 m_iShutdownStatus;
};

#include "session_server_loop.inl"

#endif

/*****************************************************************************************
	filename:	session_server_loop.h
	created:	09/27/2012
	author:		chen
	purpose:	server logic loop for session server

*****************************************************************************************/

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
struct LogicCommandPacketForward;

template<typename T>
class SessionServerLoop : public LogicLoop
{
public:
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
	void _OnCommandPacketForward(LogicCommandPacketForward*);

private:
	stdext::hash_map<uint32, T*> m_mSessionMap;
	T** m_arraySession;
	uint16 m_iSessionMax;

protected:
	ObjectPool<T> m_SessionPool;
	int32 m_iShutdownStatus;
};

#include "session_server_loop.inl"

#endif

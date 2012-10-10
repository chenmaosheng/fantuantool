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
struct LogicCommandSendData;
struct LogicCommandBroadcastData;
struct LogicCommandPacketForward;

template<typename T>
class SessionServerLoop : public LogicLoop
{
public:
	// cstr, preallocate assigned number of sessions
	SessionServerLoop(uint16 iSessionMax = 32768);
	virtual ~SessionServerLoop();

	// initialize session logic loop
	int32 Init(uint8 iServerId, ServerBase* pServer);
	// destroy session logic loop
	void Destroy();
	// clear session
	virtual void ClearSession(T* pSession);

	bool IsReadyForShutdown() const;

protected:
	T* GetSession(uint32 iSessionId);
	void _ReadyForShutdown();
	virtual bool _OnCommand(LogicCommand*);

	void _OnCommandOnConnect(LogicCommandOnConnect*);
	virtual void _OnCommandOnDisconnect(LogicCommandOnDisconnect*);
	virtual void _OnCommandOnData(LogicCommandOnData*);
	virtual void _OnCommandSendData(LogicCommandSendData*);
	void _OnCommandBroadcastData(LogicCommandBroadcastData*);
	virtual void _OnCommandShutdown();
	// handle packet forwarding to other server
	void _OnCommandPacketForward(LogicCommandPacketForward*);

protected:
	stdext::hash_map<uint32, T*> m_mSessionMap;
	T** m_arraySession;
	uint16 m_iSessionMax;

protected:
	ObjectPool<T> m_SessionPool;
	int32 m_iShutdownStatus;
};

#include "session_server_loop.inl"

#endif

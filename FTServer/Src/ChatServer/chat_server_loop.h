/*****************************************************************************************
	filename:	chat_server_loop.h
	created:	12/13/2012
	author:		chen
	purpose:	server logic loop for chat server

*****************************************************************************************/

#ifndef _H_CHAT_SERVER_LOOP
#define _H_CHAT_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include <hash_map>

class ServerBase;
class ChatSession;
struct LogicCommand;
struct LogicCommandOnConnect;
struct LogicCommandOnDisconnect;
struct LogicCommandDisconnect;
struct LogicCommandOnData;
struct LogicCommandSendData;
struct LogicCommandBroadcastData;

class ChatServerLoop : public LogicLoop
{
public:
	// cstr, preallocate assigned number of sessions
	ChatServerLoop(uint16 iSessionMax = 32768);
	~ChatServerLoop();

	// initialize chat logic loop
	int32 Init(bool bMainLoop=true);
	// destroy chat logic loop
	void Destroy();
	// clear session
	void ClearSession(ChatSession* pSession);

	bool IsReadyForShutdown() const;
	bool IsStartShutdown() const;

	int32 Start();

protected:
	ChatSession* GetSession(uint32 iSessionId);
	void _ReadyForShutdown();
	bool _OnCommand(LogicCommand*);

	DWORD _Loop();

	void _OnCommandOnConnect(LogicCommandOnConnect*);
	void _OnCommandOnDisconnect(LogicCommandOnDisconnect*);
	void _OnCommandDisconnect(LogicCommandDisconnect*);
	void _OnCommandOnData(LogicCommandOnData*);
	void _OnCommandSendData(LogicCommandSendData*);
	void _OnCommandBroadcastData(LogicCommandBroadcastData*);
	void _OnCommandShutdown();

	// check if any session disconnect, it's a heart beat test
	void _CheckSessionState();

protected:
	stdext::hash_map<uint32, ChatSession*> m_mSessionMap;
	ChatSession** m_arraySession;
	uint16 m_iSessionMax;

protected:
	ObjectPool<ChatSession> m_SessionPool;
	int32 m_iShutdownStatus;
};

#endif

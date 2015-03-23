#ifndef _H_LOGIC_LOOP
#define _H_LOGIC_LOOP

#include "common.h"
#include <list>
#include <hash_map>

struct LogicCommand;
class ServerBase;
class Session;
struct LogicCommandOnConnect;
struct LogicCommandOnDisconnect;
struct LogicCommandDisconnect;
struct LogicCommandOnData;
struct LogicCommandSendData;
class LogicLoop
{
public:
	enum { NOT_SHUTDOWN, START_SHUTDOWN, READY_FOR_SHUTDOWN, };

	// cstr and dstr
	LogicLoop();
	virtual ~LogicLoop();

	// initialize the logic loop
	virtual int32	Init(ServerBase* pServer);
	// destroy the logic loop
	virtual void	Destroy();
	// start loop
	int32			Start();
	// stop loop
	void			Stop();
	// hang up the thread
	void			Join();
	// check if ready to shutdown
	bool			IsReadyForShutdown() const;
	// get current time from server start
	DWORD			GetCurrTime() const;
	// get last loop's delta time
	DWORD			GetDeltaTime() const;
	// push logic command to queue
	void			PushCommand(LogicCommand*);
	
protected:
	DWORD			_Loop();
	virtual bool	_OnCommand(LogicCommand*);
	void			_ReadyForShutdown();

private:
	// thread run function
	static uint32 WINAPI _ThreadMain(PVOID);
	void			_OnCommandOnConnect(LogicCommandOnConnect*);
	void			_OnCommandOnDisconnect(LogicCommandOnDisconnect*);
	void			_OnCommandDisconnect(LogicCommandDisconnect*);
	void			_OnCommandOnData(LogicCommandOnData*);
	void			_OnCommandSendData(LogicCommandSendData*);
	void			_OnCommandShutdown();

private:
	HANDLE						m_hThread;
	HANDLE						m_hCommandSemaphore;
	CRITICAL_SECTION			m_csCommandList;
	std::list<LogicCommand*>	m_CommandList;
	BOOL						m_bQuit;

protected:
	static LogicLoop*			m_pMainLoop;
	// time control
	DWORD						m_dwCurrTime;	// current time, from server start on (ms)
	DWORD						m_dwDeltaTime; // unit frame time cost
	int32						m_iShutdownStatus;

protected:
	virtual Session*	_CreateSession() = 0;
	Session*			_GetSession(uint32 iSessionId);

private:
	stdext::hash_map<uint32, Session*> m_mSessionMap;
};

#endif

#ifndef _H_EASY_BASELOOP
#define _H_EASY_BASELOOP

#include "common.h"
#include <list>
#include <hash_map>

struct LogicCommand;
class EasyBaseServer;
class EasyBaseSession;
struct LogicCommandOnConnect;
struct LogicCommandOnDisconnect;
struct LogicCommandDisconnect;
struct LogicCommandOnData;
struct LogicCommandSendData;
class EasyBaseLoop
{
public:
	enum { NOT_SHUTDOWN, START_SHUTDOWN, READY_FOR_SHUTDOWN, };

	// cstr and dstr
	EasyBaseLoop();
	virtual ~EasyBaseLoop();

	// initialize the logic loop
	virtual int32	Init(EasyBaseServer* pServer);
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
	static EasyBaseLoop*			m_pMainLoop;
	// time control
	DWORD						m_dwCurrTime;	// current time, from server start on (ms)
	DWORD						m_dwDeltaTime; // unit frame time cost
	int32						m_iShutdownStatus;

protected:
	virtual EasyBaseSession*	_CreateSession() = 0;
	EasyBaseSession*			_GetSession(uint32 iSessionId);

private:
	stdext::hash_map<uint32, EasyBaseSession*> m_mSessionMap;
};

#endif

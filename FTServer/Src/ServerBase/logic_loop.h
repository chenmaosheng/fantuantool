/*****************************************************************************************
	filename:	logic_loop.h
	created:	09/27/2012
	author:		chen
	purpose:	help server to control game tick

*****************************************************************************************/

#ifndef _H_LOGIC_LOOP
#define _H_LOGIC_LOOP

#include "server_common.h"
#include <list>

// todo: what if we have more than one logicloop in one process
struct LogicCommand;
class Alarm;
class LogicLoop
{
public:
	enum { NOT_SHUTDOWN, START_SHUTDOWN, READY_FOR_SHUTDOWN, };

	// cstr and dstr
	LogicLoop();
	virtual ~LogicLoop();

	// initialize the logic loop
	virtual int32 Init();
	// destroy the logic loop
	virtual void Destroy();

	// start loop
	virtual int32 Start();
	// stop loop
	virtual void Stop();
	// check if ready to shutdown
	virtual bool IsReadyForShutdown() const = 0;

	// hang up the thread
	virtual void Join();
	
	void PushCommand(LogicCommand*);
	void PushShutdownCommand();

	// get current time from server start
	DWORD GetCurrTime() const;

	// get last loop's delta time
	DWORD GetDeltaTime() const;

	// get current world time
	// todo: if more than one loop, different loop has different world time
	uint64 GetWorldTime() const;

protected:
	virtual DWORD _Loop() = 0;
	virtual bool _OnCommand(LogicCommand*) = 0;

private:
	// thread run function
	static uint32 WINAPI _ThreadMain(PVOID);

private:
	static LogicLoop* m_pMainLoop;
	HANDLE m_hThread;
	HANDLE m_hCommandSemaphore;
	CRITICAL_SECTION m_csCommandList;
	CRITICAL_SECTION m_csLogic;
	std::list<LogicCommand*> m_CommandList;
	BOOL m_bQuit;

protected:
	// time control
	DWORD m_dwCurrTime;	// current time, from server start on (ms)
	DWORD m_dwDeltaTime; // unit frame time cost
	uint64 m_iWorldTime; // world real time (s)

	Alarm* m_pAlarm; // alarm clock
};

#endif

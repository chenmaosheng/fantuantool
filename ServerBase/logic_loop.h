#ifndef _H_LOGIC_LOOP
#define _H_LOGIC_LOOP

#include "common.h"
#include <list>


// todo: what if we have more than one logicloop in one process
struct LogicCommand;
class LogicLoop
{
public:
	enum { NOT_SHUTDOWN, START_SHUTDOWN, READY_FOR_SHUTDOWN, };

	LogicLoop();
	virtual ~LogicLoop();

	virtual int32 Init();
	virtual void Destroy();

	virtual int32 Start();
	virtual void Stop();
	virtual bool IsReadyForShutdown() const = 0;

	virtual void Join();
	
	virtual void PushCommand(LogicCommand*);
	void PushShutdownCommand();

protected:
	virtual uint32 _Loop() = 0;
	virtual bool _OnCommand(LogicCommand*) = 0;

private:
	static uint32 WINAPI _ThreadMain(PVOID);

private:
	static LogicLoop* m_pMainLoop;
	HANDLE m_hThread;
	HANDLE m_hCommandEvent;
	CRITICAL_SECTION m_csCommandList;
	std::list<LogicCommand*> m_CommandList;
	BOOL m_bQuit;
};

#endif

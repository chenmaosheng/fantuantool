#ifndef _H_LOGIC_LOOP
#define _H_LOGIC_LOOP

#include "common.h"
#include <list>

class LogicCommand;
class LogicLoop
{
public:
	LogicLoop();
	virtual ~LogicLoop();

	virtual int32 Init(bool bMainLoop = true);
	virtual void Destroy();

	virtual int32 Start();
	virtual void Stop();
	virtual bool IsAllowStop() = 0;

	virtual void Join();
	virtual void Pause();
	virtual void Resume();

	virtual void PushCommand(LogicCommand*);

protected:
	virtual DWORD _Loop() = 0;
	virtual bool _OnCommand(LogicCommand*);

private:
	static uint32 WINAPI _ThreadMain(PVOID);

private:
	static LogicLoop* m_pLogicLoop;
	HANDLE m_hThread;
	std::list<LogicCommand*> m_CommandList;
};

#endif

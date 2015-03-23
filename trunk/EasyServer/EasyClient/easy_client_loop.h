#ifndef _H_EASY_CLIENT_LOOP
#define _H_EASY_CLIENT_LOOP

#include "common.h"
#include <list>

struct LogicCommand;
class EasyClientLoop
{
public:
	EasyClientLoop();
	~EasyClientLoop();

	// initialize the logic loop
	int32 Init();
	// destroy the logic loop
	void Destroy();

	// start loop
	int32 Start();
	// stop loop
	void Stop();
	void PushCommand(LogicCommand*);

private:
	DWORD _Loop();
	bool _OnCommand(LogicCommand*);

private:
	// thread run function
	static uint32 WINAPI _ThreadMain(PVOID);

private:
	HANDLE m_hThread;
	HANDLE m_hCommandSemaphore;
	std::list<LogicCommand*> m_CommandList;
	CRITICAL_SECTION m_csCommandList;
	CRITICAL_SECTION m_csLogic;
	BOOL m_bQuit;
};

#endif


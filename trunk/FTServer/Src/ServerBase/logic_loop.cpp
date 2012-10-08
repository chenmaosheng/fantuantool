#include "logic_loop.h"
#include "logic_command.h"
#include "alarm.h"
#include <process.h>
#include <mmsystem.h>
#include <ctime>

LogicLoop* LogicLoop::m_pMainLoop = NULL;

LogicLoop::LogicLoop()
{
	m_hThread = NULL;
	m_hCommandEvent = NULL;
	m_bQuit = 0;
	m_dwCurrTime = 0;
	m_dwDeltaTime = 0;
	m_iWorldTime = 0;
	m_pAlarm = new Alarm;
	InitializeCriticalSection(&m_csCommandList);
}

LogicLoop::~LogicLoop()
{
	SAFE_DELETE(m_pAlarm);
	DeleteCriticalSection(&m_csCommandList);
	CloseHandle(m_hCommandEvent);
	CloseHandle(m_hThread);
}

int32 LogicLoop::Init()
{
	if (!m_pMainLoop)
	{
		m_pMainLoop = this;
		return 0;
	}

	return -1;
}

void LogicLoop::Destroy()
{
	while (!m_CommandList.empty())
	{
		SAFE_DELETE(m_CommandList.front());
		m_CommandList.pop_front();
	}
}

int32 LogicLoop::Start()
{
	// start time line
	m_dwCurrTime = timeGetTime();
	m_dwDeltaTime = 0;

	// start alarm clock
	m_pAlarm->Start(m_dwCurrTime);

	// create an event to control command push and pop
	m_hCommandEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	// create a thread to handle command
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &LogicLoop::_ThreadMain, this, 0, NULL);

	return 0;
}

void LogicLoop::Stop()
{
	m_bQuit = 1;
}

void LogicLoop::Join()
{
	// let the thread join, and close the command event
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hCommandEvent);
}

void LogicLoop::PushCommand(LogicCommand* pCommand)
{
	// push asynchorous command into command list and activate the event
	EnterCriticalSection(&m_csCommandList);
	m_CommandList.push_back(pCommand);
	LeaveCriticalSection(&m_csCommandList);
	::SetEvent(m_hCommandEvent);
}

void LogicLoop::PushShutdownCommand()
{
	PushCommand(FT_NEW(LogicCommandShutdown));
}

DWORD LogicLoop::GetCurrTime() const
{
	return m_dwCurrTime;
}

DWORD LogicLoop::GetDeltaTime() const
{
	return m_dwDeltaTime;
}

uint64 LogicLoop::GetWorldTime() const
{
	return m_iWorldTime;
}

uint32 WINAPI LogicLoop::_ThreadMain(PVOID pParam)
{
	LogicLoop* pLogicLoop = (LogicLoop*)pParam;
	LogicCommand* pCommand = NULL;
	DWORD dwRet = 0;
	DWORD dwSleepTime = 0;
	DWORD dwLastTickTime = 0;

	while (!pLogicLoop->m_bQuit)
	{
		// update time control
		dwLastTickTime = pLogicLoop->m_dwCurrTime;
		pLogicLoop->m_dwCurrTime = timeGetTime();
		pLogicLoop->m_dwDeltaTime = pLogicLoop->m_dwCurrTime - dwLastTickTime;
		pLogicLoop->m_iWorldTime = time(NULL);

		while (true)
		{
			// sleep time depends on each server system
			dwRet = WaitForSingleObject(pLogicLoop->m_hCommandEvent, dwSleepTime);
			if (dwRet == WAIT_FAILED || dwRet == WAIT_TIMEOUT)
			{
				break;
			}
			else if (dwRet == WAIT_OBJECT_0)
			{
				// set sleep time to 0 first
				dwSleepTime = 0;

				// pop a command from list to be handled
				EnterCriticalSection(&pLogicLoop->m_csCommandList);
				if (!pLogicLoop->m_CommandList.empty())
				{
					pCommand = pLogicLoop->m_CommandList.front();
					pLogicLoop->m_CommandList.pop_front();
				}
				LeaveCriticalSection(&pLogicLoop->m_csCommandList);
				// deactivate the command event
				ResetEvent(pLogicLoop->m_hCommandEvent);
				pLogicLoop->_OnCommand(pCommand);

				FT_DELETE(pCommand);
			}
		}

		// update alarm clock
		pLogicLoop->m_pAlarm->Tick(pLogicLoop->m_dwCurrTime);

		// call logic loop
		dwSleepTime = pLogicLoop->_Loop();
	}

	return 0;
}
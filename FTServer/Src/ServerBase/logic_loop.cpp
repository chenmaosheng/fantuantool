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
	m_hCommandSemaphore = NULL;
	m_bQuit = 0;
	m_dwCurrTime = 0;
	m_dwDeltaTime = 0;
	m_iWorldTime = 0;
	m_pAlarm = new Alarm;
	InitializeCriticalSection(&m_csCommandList);
	InitializeCriticalSection(&m_csLogic);
}

LogicLoop::~LogicLoop()
{
	SAFE_DELETE(m_pAlarm);
	DeleteCriticalSection(&m_csLogic);
	DeleteCriticalSection(&m_csCommandList);
	CloseHandle(m_hCommandSemaphore);
	CloseHandle(m_hThread);
}

int32 LogicLoop::Init()
{
	_ASSERT(m_pMainLoop == NULL);
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

	// create a semaphore to control command push and pop
	m_hCommandSemaphore = ::CreateSemaphore(NULL, 0, MAXINT, NULL);
	if (m_hCommandSemaphore == NULL)
	{
		return -1;
	}

	// create a thread to handle command
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &LogicLoop::_ThreadMain, this, 0, NULL);
	if (m_hThread == (HANDLE)1)
	{
		CloseHandle(m_hThread);
		return -2;
	}

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
	if (m_hCommandSemaphore)
	{
		CloseHandle(m_hCommandSemaphore);
	}
	m_CommandList.clear();
}

void LogicLoop::PushCommand(LogicCommand* pCommand)
{
	BOOL bRet = 0;
	// push asynchorous command into command list and activate the event
	EnterCriticalSection(&m_csCommandList);
	m_CommandList.push_back(pCommand);
	LeaveCriticalSection(&m_csCommandList);
	bRet = ReleaseSemaphore(m_hCommandSemaphore, 1, NULL);
	if (!bRet)
	{
		_ASSERT(false);
	}
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
		EnterCriticalSection(&pLogicLoop->m_csLogic);

		// update time control
		dwLastTickTime = pLogicLoop->m_dwCurrTime;
		pLogicLoop->m_dwCurrTime = timeGetTime();
		pLogicLoop->m_dwDeltaTime = pLogicLoop->m_dwCurrTime - dwLastTickTime;
		pLogicLoop->m_iWorldTime = time(NULL);

		while (true)
		{
			// sleep time depends on each server system
			dwRet = WaitForSingleObject(pLogicLoop->m_hCommandSemaphore, dwSleepTime);
			if (dwRet == WAIT_FAILED)
			{
				_ASSERT(false && _T("failed to wait for single object"));
				break;
			}
			else
			if (dwRet == WAIT_TIMEOUT)
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
				else
				{
					_ASSERT(false && _T("Semaphore has problem"));
				}
				LeaveCriticalSection(&pLogicLoop->m_csCommandList);
				pLogicLoop->_OnCommand(pCommand);

				FT_DELETE(pCommand);
			}
		}

		// update alarm clock
		pLogicLoop->m_pAlarm->Tick(pLogicLoop->m_dwCurrTime);

		// call logic loop
		dwSleepTime = pLogicLoop->_Loop();

		PerfCounter::Instance()->Print();

		LeaveCriticalSection(&pLogicLoop->m_csLogic);
	}

	return 0;
}
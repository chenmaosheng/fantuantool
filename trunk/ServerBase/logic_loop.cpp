#include "logic_loop.h"
#include "logic_command.h"
#include <process.h>

LogicLoop* LogicLoop::m_pMainLoop = NULL;

LogicLoop::LogicLoop()
{
	m_hThread = NULL;
	m_bQuit = 0;
	InitializeCriticalSection(&m_csCommandList);
	m_hCommandEvent = NULL;
}

LogicLoop::~LogicLoop()
{
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

uint32 WINAPI LogicLoop::_ThreadMain(PVOID pParam)
{
	LogicLoop* pLogicLoop = (LogicLoop*)pParam;
	LogicCommand* pCommand = NULL;
	DWORD dwRet = 0;
	uint32 iSleepTime = 0;

	while (!pLogicLoop->m_bQuit)
	{
		while (true)
		{
			// sleep time depends on each server system
			dwRet = WaitForSingleObject(pLogicLoop->m_hCommandEvent, iSleepTime);
			if (dwRet == WAIT_FAILED || dwRet == WAIT_TIMEOUT)
			{
				break;
			}
			else if (dwRet == WAIT_OBJECT_0)
			{
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

		iSleepTime = pLogicLoop->_Loop();
	}

	return 0;
}
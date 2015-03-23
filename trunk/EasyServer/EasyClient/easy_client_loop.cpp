#include "easy_client_loop.h"
#include "easy_logic_command.h"
#include "easy_client.h"
#include "connection.h"
#include <process.h>
#include "util.h"

EasyClientLoop::EasyClientLoop()
{
	m_hThread = NULL;
	m_hCommandSemaphore = NULL;
	m_bQuit = 0;
	InitializeCriticalSection(&m_csCommandList);
	InitializeCriticalSection(&m_csLogic);
}

EasyClientLoop::~EasyClientLoop()
{
	DeleteCriticalSection(&m_csLogic);
	DeleteCriticalSection(&m_csCommandList);
	CloseHandle(m_hCommandSemaphore);
	CloseHandle(m_hThread);
}

int32 EasyClientLoop::Init()
{
	return 0;
}

void EasyClientLoop::Destroy()
{
	while (!m_CommandList.empty())
	{
		SAFE_DELETE(m_CommandList.front());
		m_CommandList.pop_front();
	}
}

int32 EasyClientLoop::Start()
{
	// create a semaphore to control command push and pop
	m_hCommandSemaphore = ::CreateSemaphore(NULL, 0, MAXINT, NULL);
	// create a thread to handle command
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &EasyClientLoop::_ThreadMain, this, 0, NULL);

	return 0;
}

void EasyClientLoop::Stop()
{
	m_bQuit = 1;
}

void EasyClientLoop::PushCommand(LogicCommand* pCommand)
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

uint32 WINAPI EasyClientLoop::_ThreadMain(PVOID pParam)
{
	EasyClientLoop* pLogicEasyClientLoop = (EasyClientLoop*)pParam;
	LogicCommand* pCommand = NULL;
	DWORD dwRet = 0;
	DWORD dwSleepTime = 0;
	DWORD dwLastTickTime = 0;

	while (!pLogicEasyClientLoop->m_bQuit)
	{
		EnterCriticalSection(&pLogicEasyClientLoop->m_csLogic);

		while (true)
		{
			// sleep time depends on each server system
			dwRet = WaitForSingleObject(pLogicEasyClientLoop->m_hCommandSemaphore, dwSleepTime);
			if (dwRet == WAIT_FAILED || dwRet == WAIT_TIMEOUT)
			{
				break;
			}
			else if (dwRet == WAIT_OBJECT_0)
			{
				// set sleep time to 0 first
				dwSleepTime = 0;

				// pop a command from list to be handled
				EnterCriticalSection(&pLogicEasyClientLoop->m_csCommandList);
				if (!pLogicEasyClientLoop->m_CommandList.empty())
				{
					pCommand = pLogicEasyClientLoop->m_CommandList.front();
					pLogicEasyClientLoop->m_CommandList.pop_front();
				}
				else
				{
					_ASSERT(false && _T("Semaphore has problem"));
				}
				LeaveCriticalSection(&pLogicEasyClientLoop->m_csCommandList);
				pLogicEasyClientLoop->_OnCommand(pCommand);

				SAFE_DELETE(pCommand);
			}
		}

		// call logic EasyClientLoop
		dwSleepTime = pLogicEasyClientLoop->_Loop();

		LeaveCriticalSection(&pLogicEasyClientLoop->m_csLogic);
	}

	return 0;
}

DWORD EasyClientLoop::_Loop()
{
	return 100;
}

bool EasyClientLoop::_OnCommand(LogicCommand* pCommand)
{
	EasyClient* pClient = g_pClient;
	switch(pCommand->m_iCmdId)
	{
		case COMMAND_ONCONNECT:
		{
			LogicCommandOnConnect* pCommandOnConnect = (LogicCommandOnConnect*)pCommand;
			pClient->OnClientConnection(pCommandOnConnect->m_ConnId);
		}
		break;

		case COMMAND_ONDISCONNECT:
		{
			LogicCommandOnDisconnect* pCommandOnDisconnect = (LogicCommandOnDisconnect*)pCommand;
			pClient->OnClientDisconnect(pCommandOnDisconnect->m_ConnId);
		}
		break;

		case COMMAND_ONDATA:
		{
			LogicCommandOnData* pCommandOnData = (LogicCommandOnData*)pCommand;
			pClient->OnClientData(pCommandOnData->m_iLen, pCommandOnData->m_pData);
		}
		break;
	}

	return true;
}
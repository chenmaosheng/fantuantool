#include "client_loop.h"
#include "client_command.h"
#include "client_base.h"
#include "connection.h"
#include <process.h>

ClientLoop::ClientLoop()
{
	m_hThread = NULL;
	m_hCommandSemaphore = NULL;
	m_bQuit = 0;
	InitializeCriticalSection(&m_csCommandList);
	InitializeCriticalSection(&m_csLogic);
}

ClientLoop::~ClientLoop()
{
	DeleteCriticalSection(&m_csLogic);
	DeleteCriticalSection(&m_csCommandList);
	CloseHandle(m_hCommandSemaphore);
	CloseHandle(m_hThread);
}

int32 ClientLoop::Init()
{
	return 0;
}

void ClientLoop::Destroy()
{
	while (!m_CommandList.empty())
	{
		SAFE_DELETE(m_CommandList.front());
		m_CommandList.pop_front();
	}
}

int32 ClientLoop::Start()
{
	// create a semaphore to control command push and pop
	m_hCommandSemaphore = ::CreateSemaphore(NULL, 0, MAXINT, NULL);
	// create a thread to handle command
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &ClientLoop::_ThreadMain, this, 0, NULL);

	return 0;
}

void ClientLoop::Stop()
{
	m_bQuit = 1;
}

void ClientLoop::PushCommand(ClientCommand* pCommand)
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

uint32 WINAPI ClientLoop::_ThreadMain(PVOID pParam)
{
	ClientLoop* pLogicLoop = (ClientLoop*)pParam;
	ClientCommand* pCommand = NULL;
	DWORD dwRet = 0;
	DWORD dwSleepTime = 0;
	DWORD dwLastTickTime = 0;

	while (!pLogicLoop->m_bQuit)
	{
		EnterCriticalSection(&pLogicLoop->m_csLogic);

		while (true)
		{
			// sleep time depends on each server system
			dwRet = WaitForSingleObject(pLogicLoop->m_hCommandSemaphore, dwSleepTime);
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
				else
				{
					_ASSERT(false && _T("Semaphore has problem"));
				}
				LeaveCriticalSection(&pLogicLoop->m_csCommandList);
				pLogicLoop->_OnCommand(pCommand);

				FT_DELETE(pCommand);
			}
		}

		// call logic loop
		dwSleepTime = pLogicLoop->_Loop();

		LeaveCriticalSection(&pLogicLoop->m_csLogic);
	}

	return 0;
}

DWORD ClientLoop::_Loop()
{
	return 100;
}

bool ClientLoop::_OnCommand(ClientCommand* pCommand)
{
	ClientBase* pClientBase = g_pClientBase;
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONCONNECT:
		{
			ClientCommandOnConnect* pCommandOnConnect = (ClientCommandOnConnect*)pCommand;
			pClientBase->OnClientConnection(pCommandOnConnect->m_ConnId);
		}
		break;

	case COMMAND_ONDISCONNECT:
		{
			ClientCommandOnDisconnect* pCommandOnDisconnect = (ClientCommandOnDisconnect*)pCommand;
			pClientBase->OnClientDisconnect(pCommandOnDisconnect->m_ConnId);
		}
		break;

	case COMMAND_ONDATA:
		{
			ClientCommandOnData* pCommandOnData = (ClientCommandOnData*)pCommand;
			pClientBase->OnClientData(pCommandOnData->m_iLen, pCommandOnData->m_pData);
		}
		break;
	}

	return true;
}
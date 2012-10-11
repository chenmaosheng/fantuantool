#include "client_loop.h"
#include "client_command.h"
#include "client_base.h"
#include "connection.h"
#include <process.h>

ClientLoop::ClientLoop()
{
	m_hThread = NULL;
	m_hCommandEvent = NULL;
	m_bQuit = 0;
	InitializeCriticalSection(&m_csCommandList);
}

ClientLoop::~ClientLoop()
{
	DeleteCriticalSection(&m_csCommandList);
	CloseHandle(m_hCommandEvent);
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
	// create an event to control command push and pop
	m_hCommandEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
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
	// push asynchorous command into command list and activate the event
	EnterCriticalSection(&m_csCommandList);
	m_CommandList.push_back(pCommand);
	LeaveCriticalSection(&m_csCommandList);
	::SetEvent(m_hCommandEvent);
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

		// call logic loop
		dwSleepTime = pLogicLoop->_Loop();
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
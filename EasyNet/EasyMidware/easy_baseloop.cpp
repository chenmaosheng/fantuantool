#include "easy_baseloop.h"
#include "easy_basecommand.h"
#include "easy_basesession.h"
#include "easy_baseserver.h"
#include "easy_connection.h"
#include "util.h"
#include <process.h>
#include <mmsystem.h>
#include <ctime>
#include "easy_log.h"

EasyBaseLoop* EasyBaseLoop::m_pMainLoop = NULL;

EasyBaseLoop::EasyBaseLoop()
{
	m_hThread = NULL;
	m_hCommandSemaphore = NULL;
	m_bQuit = 0;
	m_dwCurrTime = 0;
	m_dwDeltaTime = 0;
	InitializeCriticalSection(&m_csCommandList);
	m_pMainLoop = this;
}

EasyBaseLoop::~EasyBaseLoop()
{
	while (!m_CommandList.empty())
	{
		SAFE_DELETE(m_CommandList.front());
		m_CommandList.pop_front();
	}

	DeleteCriticalSection(&m_csCommandList);
	CloseHandle(m_hCommandSemaphore);
	CloseHandle(m_hThread);
}

int32 EasyBaseLoop::Start()
{
	// start time line
	m_dwCurrTime = timeGetTime();
	m_dwDeltaTime = 0;

	// create a semaphore to control command push and pop
	m_hCommandSemaphore = ::CreateSemaphore(NULL, 0, MAXINT, NULL);
	if (m_hCommandSemaphore == NULL)
	{
		return -1;
	}

	// create a thread to handle command
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &EasyBaseLoop::_ThreadMain, this, 0, NULL);
	if (m_hThread == (HANDLE)1)
	{
		CloseHandle(m_hThread);
		return -2;
	}

	return 0;
}

void EasyBaseLoop::Stop()
{
	m_bQuit = 1;
}

void EasyBaseLoop::Join()
{
	// let the thread join, and close the command event
	WaitForSingleObject(m_hThread, INFINITE);
	if (m_hCommandSemaphore)
	{
		CloseHandle(m_hCommandSemaphore);
	}
	m_CommandList.clear();
}

DWORD EasyBaseLoop::GetCurrTime() const
{
	return m_dwCurrTime;
}

DWORD EasyBaseLoop::GetDeltaTime() const
{
	return m_dwDeltaTime;
}

void EasyBaseLoop::PushCommand(LogicCommand* pCommand)
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

DWORD EasyBaseLoop::_Loop()
{
	return 100;
}

uint32 WINAPI EasyBaseLoop::_ThreadMain(PVOID pParam)
{
	EasyBaseLoop* pEasyBaseLoop = (EasyBaseLoop*)pParam;
	LogicCommand* pCommand = NULL;
	DWORD dwRet = 0;
	DWORD dwSleepTime = 0;
	DWORD dwLastTickTime = 0;

	while (!pEasyBaseLoop->m_bQuit)
	{
		// update time control
		dwLastTickTime = pEasyBaseLoop->m_dwCurrTime;
		pEasyBaseLoop->m_dwCurrTime = timeGetTime();
		pEasyBaseLoop->m_dwDeltaTime = pEasyBaseLoop->m_dwCurrTime - dwLastTickTime;
		
		while (true)
		{
			// sleep time depends on each server system
			dwRet = WaitForSingleObject(pEasyBaseLoop->m_hCommandSemaphore, dwSleepTime);
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
				EnterCriticalSection(&pEasyBaseLoop->m_csCommandList);
				if (!pEasyBaseLoop->m_CommandList.empty())
				{
					pCommand = pEasyBaseLoop->m_CommandList.front();
					pEasyBaseLoop->m_CommandList.pop_front();
				}
				else
				{
					_ASSERT(false && _T("Semaphore has problem"));
				}
				LeaveCriticalSection(&pEasyBaseLoop->m_csCommandList);
				pEasyBaseLoop->_OnCommand(pCommand);

				SAFE_DELETE(pCommand);
			}
		}

		// call logic loop
		dwSleepTime = pEasyBaseLoop->_Loop();
	}

	return 0;
}

bool EasyBaseLoop::_OnCommand(LogicCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONCONNECT:
		_OnCommandOnConnect((LogicCommandOnConnect*)pCommand);
		break;

	case COMMAND_ONDISCONNECT:
		_OnCommandOnDisconnect((LogicCommandOnDisconnect*)pCommand);
		break;

	case COMMAND_DISCONNECT:
		_OnCommandDisconnect((LogicCommandDisconnect*)pCommand);
		break;

	case COMMAND_ONDATA:
		_OnCommandOnData((LogicCommandOnData*)pCommand);
		break;

	case COMMAND_SENDDATA:
		_OnCommandSendData((LogicCommandSendData*)pCommand);
		break;

	case COMMAND_SHUTDOWN:
		_OnCommandShutdown();
		break;

	default:
		LOG_ERR(_T("Undefined command id=%d"), pCommand->m_iCmdId);
		break;
	}

	return true;
}

void EasyBaseLoop::_OnCommandOnConnect(LogicCommandOnConnect* pCommand)
{
	EasyBaseSession* pSession = _CreateSession();
	m_mSessionMap.insert(std::make_pair(pSession->m_iSessionId, pSession));
	pSession->OnConnection(pCommand->m_ConnId);       
}

void EasyBaseLoop::_OnCommandOnDisconnect(LogicCommandOnDisconnect* pCommand)
{
	EasyConnection* pConnection = (EasyConnection*)pCommand->m_ConnId;
	EasyBaseSession* pSession = (EasyBaseSession*)pConnection->client_;
	if (pSession)
	{
		stdext::hash_map<uint32, EasyBaseSession*>::iterator mit = m_mSessionMap.find(pSession->m_iSessionId);
		if (mit != m_mSessionMap.end())
		{
			m_mSessionMap.erase(mit);
		}

		pSession->OnDisconnect();
	}
}

void EasyBaseLoop::_OnCommandDisconnect(LogicCommandDisconnect* pCommand)
{
	EasyBaseSession* pSession = (EasyBaseSession*)_GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		pSession->Disconnect();
	}
}

void EasyBaseLoop::_OnCommandOnData(LogicCommandOnData* pCommand)
{
	EasyConnection* pConnection = (EasyConnection*)pCommand->m_ConnId;
	EasyBaseSession* pSession = (EasyBaseSession*)pConnection->client_;
	if (pSession)
	{
		pSession->OnData(pCommand->m_iLen, pCommand->m_pData);
	}
	else
	{
		LOG_ERR(_T("Session can't be found"));
		AsyncDisconnect(pConnection);
	}
}

void EasyBaseLoop::_OnCommandSendData(LogicCommandSendData* pCommand)
{
	int32 iRet = 0;
	EasyBaseSession* pSession = (EasyBaseSession*)_GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		iRet = pSession->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData);
		if (iRet != 0)
		{
			LOG_ERR(_T("sid=%08x senddata failed"), pCommand->m_iSessionId);
		}
	}
}

void EasyBaseLoop::_OnCommandShutdown()
{
	for (stdext::hash_map<uint32, EasyBaseSession*>::iterator mit = m_mSessionMap.begin();
			mit != m_mSessionMap.end(); ++mit)
	{
		mit->second->Disconnect();
	}
}

EasyBaseSession* EasyBaseLoop::_GetSession(uint32 iSessionId)
{
	stdext::hash_map<uint32, EasyBaseSession*>::iterator mit = m_mSessionMap.find(iSessionId);
	if (mit != m_mSessionMap.end())
	{
		return mit->second;
	}

	return NULL;
}


#include "logic_loop.h"
#include "logic_command.h"
#include "session.h"
#include "server_base.h"
#include "connection.h"
#include "util.h"
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
	m_iShutdownStatus = NOT_SHUTDOWN;
	InitializeCriticalSection(&m_csCommandList);
}

LogicLoop::~LogicLoop()
{
	DeleteCriticalSection(&m_csCommandList);
	CloseHandle(m_hCommandSemaphore);
	CloseHandle(m_hThread);
}

int32 LogicLoop::Init(ServerBase* pServer)
{
	_ASSERT(m_pMainLoop == NULL);
	if (m_pMainLoop)
	{
		return -1;
	}

	m_pMainLoop = this;
	Session::Initialize(pServer);
	return 0;
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

DWORD LogicLoop::GetCurrTime() const
{
	return m_dwCurrTime;
}

DWORD LogicLoop::GetDeltaTime() const
{
	return m_dwDeltaTime;
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

bool LogicLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

void LogicLoop::_ReadyForShutdown()
{
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		m_iShutdownStatus = READY_FOR_SHUTDOWN;
	}
}

DWORD LogicLoop::_Loop()
{
	// check if ready for shutdown
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		_ReadyForShutdown();
	}

	return 100;
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

				SAFE_DELETE(pCommand);
			}
		}

		// call logic loop
		dwSleepTime = pLogicLoop->_Loop();
	}

	return 0;
}

bool LogicLoop::_OnCommand(LogicCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONCONNECT:
		if (m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandOnConnect((LogicCommandOnConnect*)pCommand);
		}
		break;

	case COMMAND_ONDISCONNECT:
		_OnCommandOnDisconnect((LogicCommandOnDisconnect*)pCommand);
		break;

	case COMMAND_DISCONNECT:
		_OnCommandDisconnect((LogicCommandDisconnect*)pCommand);
		break;

	case COMMAND_ONDATA:
		if (m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandOnData((LogicCommandOnData*)pCommand);
		}
		break;

	case COMMAND_SENDDATA:
		if (m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandSendData((LogicCommandSendData*)pCommand);
		}
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

void LogicLoop::_OnCommandOnConnect(LogicCommandOnConnect* pCommand)
{
	Session* pSession = _CreateSession();
	m_mSessionMap.insert(std::make_pair(pSession->m_iSessionId, pSession));
	pSession->OnConnection(pCommand->m_ConnId);       
}

void LogicLoop::_OnCommandOnDisconnect(LogicCommandOnDisconnect* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	Session* pSession = (Session*)pConnection->client_;
	if (pSession)
	{
		stdext::hash_map<uint32, Session*>::iterator mit = m_mSessionMap.find(pSession->m_iSessionId);
		if (mit != m_mSessionMap.end())
		{
			m_mSessionMap.erase(mit);
		}

		pSession->OnDisconnect();
	}
}

void LogicLoop::_OnCommandDisconnect(LogicCommandDisconnect* pCommand)
{
	Session* pSession = (Session*)_GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		pSession->Disconnect();
	}
}

void LogicLoop::_OnCommandOnData(LogicCommandOnData* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	Session* pSession = (Session*)pConnection->client_;
	if (pSession)
	{
		pSession->OnData(pCommand->m_iLen, pCommand->m_pData);
	}
	else
	{
		LOG_ERR(_T("Session can't be found"));
		pConnection->AsyncDisconnect();
	}
}

void LogicLoop::_OnCommandSendData(LogicCommandSendData* pCommand)
{
	int32 iRet = 0;
	Session* pSession = (Session*)_GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		iRet = pSession->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData);
		if (iRet != 0)
		{
			LOG_ERR(_T("sid=%08x senddata failed"), pCommand->m_iSessionId);
		}
	}
}

void LogicLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;

	for (stdext::hash_map<uint32, Session*>::iterator mit = m_mSessionMap.begin();
			mit != m_mSessionMap.end(); ++mit)
	{
		mit->second->Disconnect();
	}
}

Session* LogicLoop::_GetSession(uint32 iSessionId)
{
	stdext::hash_map<uint32, Session*>::iterator mit = m_mSessionMap.find(iSessionId);
	if (mit != m_mSessionMap.end())
	{
		return mit->second;
	}

	return NULL;
}


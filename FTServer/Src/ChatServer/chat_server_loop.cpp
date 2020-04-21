#include "chat_server_loop.h"
#include "chat_session.h"
#include "logic_command.h"
#include "connection.h"
#include "chat_server.h"
#include "packet.h"
#include "alarm.h"
#include "chat_server_config.h"

ChatServerLoop::ChatServerLoop(uint16 iSessionMax) :
m_SessionPool(iSessionMax),
m_iShutdownStatus(NOT_SHUTDOWN)
{
	m_iSessionMax = iSessionMax;
	m_arraySession = new ChatSession*[iSessionMax];
}

ChatServerLoop::~ChatServerLoop()
{
	SAFE_DELETE_ARRAY(m_arraySession);
}

int32 ChatServerLoop::Init(bool bMainLoop)
{
	ChatSession* pSession = NULL;
	int32 iRet = 0;
	SessionId id;

	LogicLoop::Init();

	id.iValue_ = 0;
	id.sValue_.serverId_ = g_pServerConfig->m_iServerId;

	// first initialize all session and put them into pool
	// that's why we can't initialize session in pool itself
	for (uint16 i = 0; i < m_iSessionMax; ++i)
	{
		pSession = m_SessionPool.Allocate();
		if (!pSession)
		{
			LOG_ERR(LOG_SERVER, _T("Allocate session failed"));
			return -1;
		}

		id.sValue_.session_index_ = i;
		pSession->m_iSessionId = id.iValue_;
		m_arraySession[i] = pSession;
	}

	// free all the session to pool, and wait for allocation
	for (uint16 i = 0; i < m_iSessionMax; ++i)
	{
		m_SessionPool.Free(m_arraySession[i]);
	}

	LOG_STT(LOG_SERVER, _T("Initialize session pool success"));

	ChatSession::m_pMainLoop = this;

	ChatSession::Initialize(g_pServer);
	LOG_STT(LOG_SERVER, _T("Initialize session server success"));

	return 0;
}

void ChatServerLoop::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Destroy chat server success"));
}

void ChatServerLoop::ClearSession(ChatSession* pSession)
{
	stdext::hash_map<uint32, ChatSession*>::iterator mit = m_mSessionMap.find(pSession->m_iSessionId);
	if (mit != m_mSessionMap.end())
	{
		m_mSessionMap.erase(mit);
	}
}

ChatSession* ChatServerLoop::GetSession(uint32 iSessionId)
{
	SessionId id;
	ChatSession* pSession = NULL;
	id.iValue_ = iSessionId;

	if (id.sValue_.session_index_ > m_iSessionMax)
	{
		LOG_ERR(LOG_SERVER, _T("SessionId is invalid, sid=%08x"), iSessionId);
		return NULL;
	}

	pSession = m_arraySession[id.sValue_.session_index_];
	if (pSession->m_iSessionId != iSessionId)
	{
		LOG_ERR(LOG_SERVER, _T("SessionId mismatched, m_iSessionId=%d, iSessionId=%d"), pSession->m_iSessionId, iSessionId);
		return NULL;
	}

	return pSession;
}

bool ChatServerLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

bool ChatServerLoop::IsStartShutdown() const
{
	return m_iShutdownStatus == START_SHUTDOWN;
}

bool ChatServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONCONNECT:
		if ( m_iShutdownStatus <= NOT_SHUTDOWN)
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
		if ( m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandOnData((LogicCommandOnData*)pCommand);
		}
		break;

	case COMMAND_SENDDATA:
		if ( m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandSendData((LogicCommandSendData*)pCommand);
		}
		break;

	case COMMAND_BROADCASTDATA:
		if ( m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandBroadcastData((LogicCommandBroadcastData*)pCommand);
		}
		break;

	case COMMAND_SHUTDOWN:
		_OnCommandShutdown();
		break;

	default:
		LOG_ERR(LOG_SERVER, _T("Undefined command id=%d"), pCommand->m_iCmdId);
		break;
	}

	return true;
}

void ChatServerLoop::_OnCommandOnConnect(LogicCommandOnConnect* pCommand)
{
	ChatSession* pSession = m_SessionPool.Allocate();
	if (pSession)
	{
		m_mSessionMap.insert(std::make_pair(pSession->m_iSessionId, pSession));
		pSession->OnConnection(pCommand->m_ConnId);
	}
	else
	{
		LOG_ERR(LOG_SERVER, _T("Allocate session failed"));
		((Connection*)pCommand->m_ConnId)->AsyncDisconnect();
	}
}

void ChatServerLoop::_OnCommandOnDisconnect(LogicCommandOnDisconnect* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	ChatSession* pSession = (ChatSession*)pConnection->client_;
	if (pSession)
	{
		stdext::hash_map<uint32, ChatSession*>::iterator mit = m_mSessionMap.find(pSession->m_iSessionId);
		if (mit != m_mSessionMap.end())
		{
			m_mSessionMap.erase(mit);
		}

		pSession->OnDisconnect();
		m_SessionPool.Free(pSession);
	}
}

void ChatServerLoop::_OnCommandDisconnect(LogicCommandDisconnect* pCommand)
{
	ChatSession* pSession = (ChatSession*)GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		pSession->Disconnect();
	}
}

void ChatServerLoop::_OnCommandOnData(LogicCommandOnData* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	ChatSession* pSession = (ChatSession*)pConnection->client_;
	if (pSession)
	{
		pSession->OnData(pCommand->m_iLen, pCommand->m_pData);
	}
	else
	{
		LOG_ERR(LOG_SERVER, _T("Session can't be found"));
		pConnection->AsyncDisconnect();
	}
}

void ChatServerLoop::_OnCommandSendData(LogicCommandSendData* pCommand)
{
	int32 iRet = 0;
	ChatSession* pSession = (ChatSession*)GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		iRet = pSession->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("sid=%08x senddata failed"), pCommand->m_iSessionId);
		}
	}
}

void ChatServerLoop::_OnCommandBroadcastData(LogicCommandBroadcastData* pCommand)
{
	ChatSession* pSession = NULL;
	for (uint16 i = 0; i < pCommand->m_iSessionCount; ++i)
	{
		pSession = (ChatSession*)GetSession(pCommand->m_arraySessionId[i]);
		if (pSession)
		{
			if (pSession->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData) != 0)
			{
				LOG_ERR(LOG_SERVER, _T("sid=%08x broadcastdata failed"), pSession->m_iSessionId);
			}
		}
	}
}

void ChatServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;

	for (stdext::hash_map<uint32, ChatSession*>::iterator mit = m_mSessionMap.begin();
		mit != m_mSessionMap.end(); ++mit)
	{
		mit->second->Disconnect();
	}
}

void ChatServerLoop::_ReadyForShutdown()
{
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		// todo: check if each session has been disconnected
		m_iShutdownStatus = READY_FOR_SHUTDOWN;
	}
}

int32 ChatServerLoop::Start()
{
	int32 iRet = 0;

	iRet = LogicLoop::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	// register alarm event
	m_pAlarm->RegisterEvent(_T("CheckSession"), m_dwCurrTime, g_pServerConfig->m_iCheckSessionInterval, this, &ChatServerLoop::_CheckSessionState);
	
	return 0;
}

DWORD ChatServerLoop::_Loop()
{
	// check if ready for shutdown
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		_ReadyForShutdown();
	}

	return 100;
}

void ChatServerLoop::_CheckSessionState()
{
	ChatSession* pSession = NULL;
	DWORD dwCurrTime = GetCurrTime();

	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return;
	}

	for (uint16 i = 0; i < m_iSessionMax; ++i)
	{
		pSession = m_arraySession[i];
		if (pSession->m_StateMachine.GetCurrState() == SESSION_STATE_NONE)
		{
			continue;
		}

		switch(pSession->m_StateMachine.GetCurrState())
		{
		case SESSION_STATE_ONCONNECTION:
			{
				if (pSession->m_dwConnectionTime + g_pServerConfig->m_iConnectionTimeout <= dwCurrTime)
				{
					LOG_DBG(LOG_SERVER, _T("sid=%08x Connection timeout"), pSession->m_iSessionId);
					pSession->Disconnect();
				}
			}
			break;

		case SESSION_STATE_LOGGEDIN:
			{
				if (pSession->m_dwLoggedInTime + g_pServerConfig->m_iChatTimeout <= dwCurrTime)
				{
					LOG_DBG(LOG_SERVER, _T("sid=%08x Login timeout"), pSession->m_iSessionId);
					pSession->Disconnect();
				}
			}
			break;
		}
	}
}

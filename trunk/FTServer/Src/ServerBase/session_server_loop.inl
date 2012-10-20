#include "session_server_loop.h"
#include "session.h"
#include "logic_command.h"
#include "connection.h"
#include "session_server.h"
#include "packet.h"

template<typename T>
SessionServerLoop<T>::SessionServerLoop(uint16 iSessionMax) :
m_SessionPool(iSessionMax),
m_iShutdownStatus(NOT_SHUTDOWN)
{
	m_iSessionMax = iSessionMax;
	m_arraySession = new T*[iSessionMax];
}

template<typename T>
SessionServerLoop<T>::~SessionServerLoop()
{
	SAFE_DELETE_ARRAY(m_arraySession);
}

template<typename T>
int32 SessionServerLoop<T>::Init(uint8 iServerId, TCHAR* strPrivateKey, ServerBase* pServer)
{
	T* pSession = NULL;
	int32 iRet = 0;
	SessionId id;

	LogicLoop::Init();

	id.iValue_ = 0;
	id.sValue_.serverId_ = iServerId;

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

	// load private key file
	Session::Initialize(strPrivateKey, pServer);
	LOG_STT(LOG_SERVER, _T("Initialize session server success"));

	return 0;
}

template<typename T>
void SessionServerLoop<T>::Destroy()
{
	LOG_STT(LOG_SERVER, _T("Destroy session server success"));
}

template<typename T>
void SessionServerLoop<T>::ClearSession(T* pSession)
{
	stdext::hash_map<uint32, T*>::iterator mit = m_mSessionMap.find(pSession->m_iSessionId);
	if (mit != m_mSessionMap.end())
	{
		m_mSessionMap.erase(mit);
	}
}

template<typename T>
T* SessionServerLoop<T>::GetSession(uint32 iSessionId)
{
	SessionId id;
	T* pSession = NULL;
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

template<typename T>
bool SessionServerLoop<T>::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

template<typename T>
bool SessionServerLoop<T>::_OnCommand(LogicCommand* pCommand)
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

	case COMMAND_PACKETFORWARD:
		if ( m_iShutdownStatus <= NOT_SHUTDOWN)
		{
			_OnCommandPacketForward((LogicCommandPacketForward*)pCommand);
		}
		break;

	default:
		LOG_ERR(LOG_SERVER, _T("Undefined command id=%d"), pCommand->m_iCmdId);
		break;
	}

	return true;
}

template<typename T>
void SessionServerLoop<T>::_OnCommandOnConnect(LogicCommandOnConnect* pCommand)
{
	T* pSession = m_SessionPool.Allocate();
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

template<typename T>
void SessionServerLoop<T>::_OnCommandOnDisconnect(LogicCommandOnDisconnect* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	T* pSession = (T*)pConnection->client_;
	if (pSession)
	{
		stdext::hash_map<uint32, T*>::iterator mit = m_mSessionMap.find(pSession->m_iSessionId);
		if (mit != m_mSessionMap.end())
		{
			m_mSessionMap.erase(mit);
		}

		pSession->OnDisconnect();
		m_SessionPool.Free(pSession);
	}
}

template<typename T>
void SessionServerLoop<T>::_OnCommandOnData(LogicCommandOnData* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	T* pSession = (T*)pConnection->client_;
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

template<typename T>
void SessionServerLoop<T>::_OnCommandSendData(LogicCommandSendData* pCommand)
{
	int32 iRet = 0;
	T* pSession = (T*)GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		iRet = pSession->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("sid=%08x senddata failed"), pCommand->m_iSessionId);
		}
	}
}

template<typename T>
void SessionServerLoop<T>::_OnCommandBroadcastData(LogicCommandBroadcastData* pCommand)
{
	T* pSession = NULL;
	for (uint16 i = 0; i < pCommand->m_iSessionCount; ++i)
	{
		pSession = (T*)GetSession(pCommand->m_arraySessionId[i]);
		if (pSession)
		{
			if (pSession->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData) != 0)
			{
				LOG_ERR(LOG_SERVER, _T("sid=%08x broadcastdata failed"), pSession->m_iSessionId);
			}
		}
	}
}

template<typename T>
void SessionServerLoop<T>::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;

	for (stdext::hash_map<uint32, T*>::iterator mit = m_mSessionMap.begin();
		mit != m_mSessionMap.end(); ++mit)
	{
		mit->second->Disconnect();
	}
}

template<typename T>
void SessionServerLoop<T>::_OnCommandPacketForward(LogicCommandPacketForward* pCommand)
{
	LOG_DBG(LOG_SERVER, _T(""));
	T* pSession = GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		if (Sender::SendPacket(pSession, pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData) != 0)
		{
			LOG_ERR(LOG_SERVER, _T("SendPacket failed, sid=%08x"), pCommand->m_iSessionId);
		}
	}
	else
	{
		LOG_ERR(LOG_SERVER, _T("Can't find session, sid=%08x"), pCommand->m_iSessionId);
	}
}

template<typename T>
void SessionServerLoop<T>::_ReadyForShutdown()
{
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		// todo: check if each session has been disconnected
		m_iShutdownStatus = READY_FOR_SHUTDOWN;
	}
}
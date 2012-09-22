#include "server_loop.h"
#include "session.h"
#include "server_base.h"
#include "logic_command.h"
#include "connection.h"
#include "server.h"

ServerLoop::ServerLoop(uint16 iSessionMax) :
m_SessionPool(iSessionMax)
{
	m_iSessionMax = iSessionMax;
	m_arraySession = new Session*[iSessionMax];
}

ServerLoop::~ServerLoop()
{
	SAFE_DELETE_ARRAY(m_arraySession);
}

int32 ServerLoop::Init()
{
	Session* pSession = NULL;
	int32 iRet = 0;
	SessionId id;

	LogicLoop::Init();

	id.iValue_ = 0;
	id.sValue_.serverId_ = 0; // todo: where is server id

	// first initialize all session and put them into pool
	// that's why we can't initialize session in pool itself
	for (uint16 i = 0; i < m_iSessionMax; ++i)
	{
		pSession = m_SessionPool.Allocate();
		if (!pSession)
		{
			return -1;
		}

		id.sValue_.session_index_ = i;
		pSession->m_iSessionId = id.iValue_;
		m_arraySession[i] = pSession;
	}

	for (uint16 i = 0; i < m_iSessionMax; ++i)
	{
		m_SessionPool.Free(m_arraySession[i]);
	}

	Session::Initialize(g_pServer);

	return 0;
}

void ServerLoop::Destroy()
{

}

Session* ServerLoop::GetSession(uint32 iSessionId)
{
	SessionId id;
	Session* pSession = NULL;
	id.iValue_ = iSessionId;

	pSession = m_arraySession[id.sValue_.session_index_];
	if (pSession->m_iSessionId != iSessionId)
	{
		return NULL;
	}

	return pSession;
}

DWORD ServerLoop::_Loop()
{
	return 0;
}

bool ServerLoop::_OnCommand(LogicCommand* pCommand)
{
	switch(pCommand->m_iCmdId)
	{
	case COMMAND_ONCONNECT:
		_OnCommandOnConnect((LogicCommandOnConnect*)pCommand);
		break;

	case COMMAND_ONDISCONNECT:
		_OnCommandOnDisconnect((LogicCommandOnDisconnect*)pCommand);
		break;

	case COMMAND_ONDATA:
		_OnCommandOnData((LogicCommandOnData*)pCommand);
		break;

	case COMMAND_BROADCASTDATA:
		_OnCommandBroadcastData((LogicCommandBroadcastData*)pCommand);
		break;
	}

	return true;
}

void ServerLoop::_OnCommandOnConnect(LogicCommandOnConnect* pCommand)
{
	Session* pSession = m_SessionPool.Allocate();
	if (pSession)
	{
		m_mSessionMap.insert(std::make_pair(pSession->m_iSessionId, pSession));
		pSession->OnConnection(pCommand->m_ConnId);
	}
	else
	{
		((Connection*)pCommand->m_ConnId)->AsyncDisconnect();
	}
}

void ServerLoop::_OnCommandOnDisconnect(LogicCommandOnDisconnect* pCommand)
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
		m_SessionPool.Free(pSession);
	}
	else
	{
		pConnection->AsyncDisconnect();
	}
}

void ServerLoop::_OnCommandOnData(LogicCommandOnData* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	Session* pSession = (Session*)pConnection->client_;
	if (pSession)
	{
		pSession->OnData(pCommand->m_iLen, pCommand->m_pData);
	}
	else
	{
		pConnection->AsyncDisconnect();
	}
}

void ServerLoop::_OnCommandBroadcastData(LogicCommandBroadcastData* pCommand)
{
	for (stdext::hash_map<uint32, Session*>::iterator mit = m_mSessionMap.begin();
		mit != m_mSessionMap.end(); ++mit)
	{
		mit->second->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData);
	}
}
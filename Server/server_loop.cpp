#include "server_loop.h"
#include "session.h"
#include "server_base.h"
#include "logic_command.h"
#include "connection.h"
#include "server.h"

ServerLoop::ServerLoop()
{
	// todo: where is server id
	m_iServerId = 0;
}

ServerLoop::~ServerLoop()
{

}

int32 ServerLoop::Init()
{
	LogicLoop::Init();
	Session::Initialize(g_pServer);

	return 0;
}

void ServerLoop::Destroy()
{

}

Session* ServerLoop::GetSession(uint32 iSessionId)
{
	std::map<uint32, Session*>::iterator mit = m_mSessionList.find(iSessionId);
	if (mit != m_mSessionList.end())
	{
		return mit->second;
	}

	return NULL;
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
	}

	return true;
}

void ServerLoop::_OnCommandOnConnect(LogicCommandOnConnect* pCommand)
{
	Session* pSession = new Session;
	pSession->OnConnection(pCommand->m_ConnId);

	m_mSessionList.insert(std::make_pair(pSession->m_iSessionId, pSession));
}

void ServerLoop::_OnCommandOnDisconnect(LogicCommandOnDisconnect* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	Session* pSession = (Session*)pConnection->client_;
	if (pSession)
	{
		pSession->OnDisconnect();
		std::map<uint32, Session*>::iterator mit = m_mSessionList.find(pSession->m_iSessionId);
		if (mit != m_mSessionList.end())
		{
			SAFE_DELETE(mit->second);
			m_mSessionList.erase(mit);
		}
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
}
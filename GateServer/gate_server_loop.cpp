#include "gate_server_loop.h"
#include "gate_server.h"
#include "gate_logic_command.h"
#include "gate_server_config.h"
#include "master_peer_send.h"

GateServerLoop::GateServerLoop() :
SessionServerLoop<GateSession>(g_pServerConfig->m_iSessionMax)
{
}

GateServerLoop::~GateServerLoop()
{
}

int32 GateServerLoop::Init()
{
	int32 iRet = 0;

	iRet = super::Init(g_pServerConfig->m_iServerId, g_pServer);
	if (iRet != 0)
	{
		return iRet;
	}

	GateSession::m_pMainLoop = this;

	return 0;
}

void GateServerLoop::Destroy()
{
	super::Destroy();
}

int32 GateServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

int32 GateServerLoop::TransferSession(uint32 iTempSessionId, TCHAR* strAccountName, GateSession*& pOutputSession)
{
	stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(strAccountName);
	if (mit == m_mSessionMapByName.end())
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Can't find related session info"), strAccountName, iTempSessionId);
		return -1;
	}

	GateSession* pSession = mit->second;
	GateSession* pTempSession = GetSession(iTempSessionId);

	// check state
	if (pSession->m_StateMachine.GetCurrState() != SESSION_STATE_GATEALLOCACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iTempSessionId, pSession->m_StateMachine.GetCurrState());
		return -1;
	}
	
	pSession->Clone(pTempSession);
	pSession->OnSessionTransfered();
	pOutputSession = pSession;

	return 0;
}

void GateServerLoop::CloseSession(GateSession* pSession)
{
	int32 iRet = 0;
	bool bNeedDisconnect = false;
	bool bNeedGateAllocAck = false;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x close session start"), pSession->m_strAccountName, pSession->m_iSessionId);

	switch(pSession->m_StateMachine.GetCurrState())
	{
	case SESSION_STATE_ONCONNECTION:
	case SESSION_STATE_LOGGEDIN:
	case SESSION_STATE_GATELOGINREQ:
		bNeedDisconnect = true;
		break;

	case SESSION_STATE_GATEALLOCACK:
		bNeedGateAllocAck = true;
		break;

	default:
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), pSession->m_strAccountName, pSession->m_iSessionId, pSession->m_StateMachine.GetCurrState());
		break;
	}

	if (bNeedDisconnect)
	{
		pSession->m_pConnection->AsyncDisconnect();
		return;
	}

	if (bNeedGateAllocAck)
	{
		iRet = MasterPeerSend::GateAllocAck(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId, pSession->m_iLoginSessionId, wcslen(pSession->m_strAccountName)+1, pSession->m_strAccountName, pSession->m_iSessionId);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnGateLoginReq failed"), pSession->m_strAccountName, pSession->m_iSessionId);
		}
	}

	ClearSession(pSession);
}

void GateServerLoop::ClearSession(GateSession* pSession)
{
	super::ClearSession(pSession);
	
	stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(pSession->m_strAccountName);
	if (mit != m_mSessionMapByName.end())
	{
		m_mSessionMapByName.erase(mit);
	}

	pSession->Clear();
	m_SessionPool.Free(pSession);
}

DWORD GateServerLoop::_Loop()
{
	//// check if ready for shutdown
	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		_ReadyForShutdown();
	}

	return 100;
}

bool GateServerLoop::_OnCommand(LogicCommand* pCommand)
{
	// check if ready for shutdown, then ignore all the commands
	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return true;
	}

	switch (pCommand->m_iCmdId)
	{
	case COMMAND_GATEALLOCREQ:
		_OnCommandGateAllocReq((LogicCommandGateAllocReq*)pCommand);
		break;

	case COMMAND_DISCONNECT:
		_OnCommandDisconnect((LogicCommandDisconnect*)pCommand);
		break;

	case COMMAND_GATERELEASEREQ:
		_OnCommandGateReleaseReq((LogicCommandGateReleaseReq*)pCommand);
		break;

	default:
		return super::_OnCommand(pCommand);
		break;
	}

	return true;
}

void GateServerLoop::_OnCommandGateAllocReq(LogicCommandGateAllocReq* pCommand)
{
	stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(pCommand->m_strAccountName);
	if (mit != m_mSessionMapByName.end())
	{
		LOG_ERR(LOG_SERVER, _T("Find a duplicate account on server, acc=%s"), pCommand->m_strAccountName);
		return;
	}

	GateSession* pSession = m_SessionPool.Allocate();
	m_mSessionMapByName.insert(std::make_pair(pCommand->m_strAccountName, pSession));
	pSession->OnGateAllocReq(pCommand->m_iLoginSessionId, pCommand->m_strAccountName);
}

void GateServerLoop::_OnCommandDisconnect(LogicCommandDisconnect* pCommand)
{
	GateSession* pSession = GetSession(pCommand->m_iSessionId);
	if (pSession)
	{
		pSession->OnMasterDisconnect();
	}
}

void GateServerLoop::_OnCommandGateReleaseReq(LogicCommandGateReleaseReq* pCommand)
{
	stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(pCommand->m_strAccountName);
	if (mit == m_mSessionMapByName.end())
	{
		LOG_ERR(LOG_SERVER, _T("can't find player, acc=%s"), pCommand->m_strAccountName);
		return;
	}

	mit->second->OnGateReleaseReq();
}
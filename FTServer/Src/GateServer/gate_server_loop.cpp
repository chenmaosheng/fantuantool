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

	iRet = super::Init(g_pServerConfig->m_iServerId, g_pServerConfig->m_strPrivateKey, g_pServer);
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

void GateServerLoop::CloseSession(GateSession* pSession, bool isByMaster)
{
	int32 iRet = 0;
	bool bNeedDisconnect = false;
	bool bNeedGateAllocAck = false;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x close session start"), pSession->m_strAccountName, pSession->m_iSessionId);

	if (pSession->m_bFinalizing)
	{
		return;
	}

	switch(pSession->m_StateMachine.GetCurrState())
	{
	case SESSION_STATE_ONCONNECTION:
		bNeedDisconnect = true;
		break;

	case SESSION_STATE_LOGGEDIN:
	case SESSION_STATE_GATELOGINREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			bNeedDisconnect = true;
		}
		break;

	case SESSION_STATE_GATEALLOCACK:
		bNeedGateAllocAck = true;
		break;

	case SESSION_STATE_ONDISCONNECT:
	case SESSION_STATE_TRANSFERED:
	case SESSION_STATE_GATERELEASEREQ:
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

	if (bNeedGateAllocAck && !isByMaster &&
		m_iShutdownStatus < START_SHUTDOWN)
	{
		iRet = MasterPeerSend::GateAllocAck(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId, pSession->m_iLoginSessionId, wcslen(pSession->m_strAccountName)+1, pSession->m_strAccountName, pSession->m_iSessionId);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnGateLoginReq failed"), pSession->m_strAccountName, pSession->m_iSessionId);
		}
	}

	m_SessionFinalizingQueue.push(pSession);
	pSession->m_bFinalizing = true;
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
	while (!m_SessionFinalizingQueue.empty())
	{
		ClearSession(m_SessionFinalizingQueue.back());
		m_SessionFinalizingQueue.pop();
	}

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

	case COMMAND_SENDDATA:
		_OnCommandSendData((LogicCommandSendData*)pCommand);
		break;

	case COMMAND_BROADCASTDATA:
		_OnCommandBroadcastData((LogicCommandBroadcastData*)pCommand);
		break;

	case COMMAND_GATERELEASEREQ:
		_OnCommandGateReleaseReq((LogicCommandGateReleaseReq*)pCommand);
		break;

	case COMMAND_REGIONBINDREQ:
		_OnCommandRegionBindReq((LogicCommandRegionBindReq*)pCommand);
		break;

	default:
		return super::_OnCommand(pCommand);
		break;
	}

	return true;
}

void GateServerLoop::_OnCommandOnData(LogicCommandOnData* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	GateSession* pSession = (GateSession*)pConnection->client_;
	if (!pSession || pSession->m_bFinalizing)
	{
		return;
	}

	super::_OnCommandOnData(pCommand);
}

void GateServerLoop::_OnCommandOnDisconnect(LogicCommandOnDisconnect* pCommand)
{
	Connection* pConnection = (Connection*)pCommand->m_ConnId;
	GateSession* pSession = (GateSession*)pConnection->client_;
	if (pSession)
	{
		super::ClearSession(pSession);

		pSession->OnDisconnect();
	}
}

void GateServerLoop::_OnCommandDisconnect(LogicCommandDisconnect* pCommand)
{
	GateSession* pSession = GetSession(pCommand->m_iSessionId);
	if (pSession && !pSession->m_bFinalizing)
	{
		pSession->OnMasterDisconnect();
	}
}

void GateServerLoop::_OnCommandSendData(LogicCommandSendData* pCommand)
{
	GateSession* pSession = GetSession(pCommand->m_iSessionId);
	if (!pSession || pSession->m_bFinalizing)
	{
		return;
	}

	super::_OnCommandSendData(pCommand);
}

void GateServerLoop::_OnCommandBroadcastData(LogicCommandBroadcastData* pCommand)
{
	GateSession* pSession = NULL;
	for (uint16 i = 0; i < pCommand->m_iSessionCount; ++i)
	{
		pSession = GetSession(pCommand->m_arraySessionId[i]);
		if (!pSession || 
			pSession->m_bFinalizing ||
			!pSession->m_pRegionServer)
		{
			LOG_ERR(LOG_SERVER, _T("session failed"));
			continue;
		}

		// check state
		if (pSession->m_StateMachine.StateTransition(SESSION_EVENT_SEND, false) < 0)
		{
			LOG_ERR(LOG_SERVER, _T("sid=%08x state=%d state error"), pSession->m_iSessionId, pSession->m_StateMachine.GetCurrState());
			continue;
		}

		if (pSession->SendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData) != 0)
		{
			LOG_ERR(LOG_SERVER, _T("sid=%08x broadcastdata failed"), pSession->m_iSessionId);
		}
	}
}

void GateServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;

	for (stdext::hash_map<uint32, GateSession*>::iterator mit = m_mSessionMap.begin(); mit != m_mSessionMap.end(); ++mit)
	{
		if (mit->second->m_pConnection)
		{
			mit->second->Disconnect();
		}
		else
		{
			CloseSession(mit->second);
		}
	}
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

void GateServerLoop::_OnCommandGateReleaseReq(LogicCommandGateReleaseReq* pCommand)
{
	stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(pCommand->m_strAccountName);
	if (mit == m_mSessionMapByName.end())
	{
		LOG_WAR(LOG_SERVER, _T("can't find player, acc=%s"), pCommand->m_strAccountName);
		return;
	}

	// in theory, will not arrive here, because session will receive ondisconnect first and then close the session 
	mit->second->OnGateReleaseReq();
}

void GateServerLoop::_OnCommandRegionBindReq(LogicCommandRegionBindReq* pCommand)
{
	GateSession* pSession = GetSession(pCommand->m_iSessionId);
	if (!pSession)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x can't find session"), pCommand->m_iSessionId);
		return;
	}

	pSession->OnRegionBindReq(pCommand->m_iRegionServerId);
}
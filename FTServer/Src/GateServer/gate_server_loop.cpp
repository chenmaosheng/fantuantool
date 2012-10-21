#include "gate_server_loop.h"
#include "gate_server.h"
#include "gate_logic_command.h"
#include "gate_server_config.h"
#include "alarm.h"

#include "master_peer_send.h"

GateServerLoop::GateServerLoop() :
SessionServerLoop<GateSession>(g_pServerConfig->m_iSessionMax * 2) // consider temp session
{
	m_iSessionMax = g_pServerConfig->m_iSessionMax + g_pServerConfig->m_iSessionMax / 5;
	m_iTempSessionMax = g_pServerConfig->m_iSessionMax * 2 - m_iSessionMax;
	m_iSessionCount = 0;
	m_iTempSessionCount = 0;
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

	// register alarm event
	m_pAlarm->RegisterEvent(_T("CheckSession"), m_dwCurrTime, g_pServerConfig->m_iCheckSessionInterval, this, &GateServerLoop::_CheckSessionState);
	m_pAlarm->RegisterEvent(_T("ReportState"), m_dwCurrTime, g_pServerConfig->m_iReportInterval, this, &GateServerLoop::_ReportState);

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

	if (!pSession)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Can't find session"), strAccountName, iTempSessionId);
		return -1;
	}

	// check state
	if (pSession->m_StateMachine.GetCurrState() != SESSION_STATE_GATEALLOCACK &&
		pSession->m_StateMachine.GetCurrState() != SESSION_STATE_HOLDCONNECTION)
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

	case SESSION_STATE_DISCONNECT:
	case SESSION_STATE_ONMASTERDISCONNECT:
		// todo:
		break;

	case SESSION_STATE_LOGGEDIN:
	case SESSION_STATE_GATELOGINREQ:
		if (!m_iShutdownStatus != START_SHUTDOWN)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x can't come here"), pSession->m_strAccountName, pSession->m_iSessionId);
		}
		bNeedDisconnect = true;
		break;

	case SESSION_STATE_ONGATEALLOCREQ:
		bNeedGateAllocAck = true;
		break;

	case SESSION_STATE_GATEALLOCACK:
		if (!m_iShutdownStatus != START_SHUTDOWN)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x can't come here"), pSession->m_strAccountName, pSession->m_iSessionId);
		}
		break;

	case SESSION_STATE_ONDISCONNECT:
	case SESSION_STATE_TRANSFERED:
	case SESSION_STATE_GATERELEASEREQ:
	case SESSION_STATE_HOLDCONNECTION:
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
		m_iShutdownStatus != START_SHUTDOWN)
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

	if (pSession->m_bTempSession)
	{
		m_iTempSessionCount--;
	}
	else
	{
		m_iSessionCount--;
		stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(pSession->m_strAccountName);
		if (mit != m_mSessionMapByName.end())
		{
			m_mSessionMapByName.erase(mit);
		}
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
	switch (pCommand->m_iCmdId)
	{
	case COMMAND_GATEALLOCREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandGateAllocReq((LogicCommandGateAllocReq*)pCommand);
		}
		break;

	case COMMAND_GATERELEASEREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandGateReleaseReq((LogicCommandGateReleaseReq*)pCommand);
		}
		break;

	case COMMAND_REGIONBINDREQ:
		if (m_iShutdownStatus < START_SHUTDOWN)
		{
			_OnCommandRegionBindReq((LogicCommandRegionBindReq*)pCommand);
		}
		break;

	default:
		return super::_OnCommand(pCommand);
		break;
	}

	return true;
}

void GateServerLoop::_OnCommandOnConnect(LogicCommandOnConnect* pCommand)
{
	GateSession* pSession = NULL;

	if (m_iTempSessionCount >= m_iTempSessionMax)
	{
		LOG_ERR(LOG_SERVER, _T("temp session is up to max"));
		((Connection*)pCommand->m_ConnId)->AsyncDisconnect();
		return;
	}

	pSession = m_SessionPool.Allocate();
	if (!pSession)
	{
		LOG_ERR(LOG_SERVER, _T("failed to allocate session"));
		((Connection*)pCommand->m_ConnId)->AsyncDisconnect();
		return;
	}
	
	pSession->OnConnection(pCommand->m_ConnId);
	m_iTempSessionCount++;
	return;
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

	if (pSession->m_StateMachine.StateTransition(SESSION_EVENT_SEND, false) < 0)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), pSession->m_strAccountName, pSession->m_iSessionId, pSession->m_StateMachine.GetCurrState());
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
			continue;
		}

		// check state
		if (pSession->m_StateMachine.StateTransition(SESSION_EVENT_SEND, false) < 0)
		{
			_ASSERT(false && _T("state error"));
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
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x gate alloc req"), pCommand->m_strAccountName, pCommand->m_iLoginSessionId);

	// check if account exists
	stdext::hash_map<std::wstring, GateSession*>::iterator mit = m_mSessionMapByName.find(pCommand->m_strAccountName);
	if (mit != m_mSessionMapByName.end())
	{
		_ASSERT(false && _T("Find a duplicate account on server"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Find a duplicate account on server"), pCommand->m_strAccountName, pCommand->m_iLoginSessionId);
		return;
	}

	if (m_iSessionCount >= m_iSessionMax)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x up to max session"), pCommand->m_strAccountName, pCommand->m_iLoginSessionId);
		iRet = MasterPeerSend::GateAllocAck(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId, pCommand->m_iLoginSessionId, (uint16)wcslen(pCommand->m_strAccountName)+1, pCommand->m_strAccountName, 0);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%d sid=%08x gate allocate ack failed to send"), pCommand->m_strAccountName, pCommand->m_iLoginSessionId);
		}
		return;
	}

	GateSession* pSession = m_SessionPool.Allocate();
	m_iSessionCount++;
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

void GateServerLoop::_CheckSessionState()
{
	GateSession* pSession = NULL;
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
		}
	}
}

void GateServerLoop::_ReportState()
{
	int32 iRet = 0;

	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return;
	}

	iRet = MasterPeerSend::GateReportState(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId, m_iSessionCount);
	if (iRet != 0)
	{
		_ASSERT(false && _T("GateReportState failed"));
		LOG_ERR(LOG_SERVER, _T("GateReportState failed"));
	}
}
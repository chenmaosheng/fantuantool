#include "master_server_loop.h"
#include "master_server.h"
#include "master_logic_command.h"
#include "master_player_context.h"
#include "session_peer_send.h"
#include "login_peer_send.h"
#include "gate_peer_send.h"

MasterServerLoop::MasterServerLoop() :
m_iShutdownStatus(NOT_SHUTDOWN),
m_PlayerContextPool(5000)
{
	
}

int32 MasterServerLoop::Init()
{
	int32 iRet = 0;

	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	MasterPlayerContext::m_pMainLoop = this;

	return 0;
}

void MasterServerLoop::Destroy()
{
	super::Destroy();
}

int32 MasterServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

bool MasterServerLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

void MasterServerLoop::LoginSession2GateSession(MasterPlayerContext* pPlayerContext, uint32 iLoginSessionId, uint32 iGateSessionId)
{
	// todo:
}

int32 MasterServerLoop::GateAllocReq()
{
	// todo:
	return 2;
}

void MasterServerLoop::ShutdownPlayer(MasterPlayerContext* pPlayerContext)
{
	int32 iRet = 0;
	bool bSendOnLoginAck = false;
	bool bSendGateReleaseReq = false;
	bool bSendGateDisconnect = false;

	if (pPlayerContext->m_bFinalizing)
	{
		return;
	}

	switch (pPlayerContext->m_StateMachine.GetCurrState())
	{
	case PLAYER_STATE_ONLOGINREQ:
		bSendOnLoginAck = true;
		break;

	case PLAYER_STATE_GATEALLOCREQ:
	case PLAYER_STATE_GATEALLOCACK:
		bSendOnLoginAck = true;
		bSendGateReleaseReq = true;
		break;

	case PLAYER_STATE_GATEALLOCNTF:
		bSendGateReleaseReq = true;
		break;

	case PLAYER_STATE_ONGATELOGINREQ:
		bSendGateDisconnect = true;
		break;
	}

	if (bSendOnLoginAck && !IsReadyForShutdown())
	{
		LoginPeerSend::OnLoginAck(g_pServer->m_pLoginServer, pPlayerContext->m_iSessionId, 3);
	}

	if (bSendGateReleaseReq && !IsReadyForShutdown())
	{
		GatePeerSend::GateReleaseReq(g_pServer->GetPeerServer(pPlayerContext->m_iGateServerId), pPlayerContext->m_iSessionId, wcslen(pPlayerContext->m_strAccountName)+1, pPlayerContext->m_strAccountName);
	}

	if (bSendGateDisconnect && !IsReadyForShutdown())
	{
		SessionPeerSend::Disconnect(g_pServer->GetPeerServer(pPlayerContext->m_iGateServerId), pPlayerContext->m_iSessionId, 0);
	}

	AddPlayerToFinalizingQueue(pPlayerContext);
}

void MasterServerLoop::AddPlayerToFinalizingQueue(MasterPlayerContext* pPlayerContext)
{
	pPlayerContext->m_bFinalizing = true;
	m_PlayerFinalizingQueue.push(pPlayerContext);
}

void MasterServerLoop::DeletePlayer(MasterPlayerContext* pPlayerContext)
{
	stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.find(pPlayerContext->m_strAccountName);
	if (mit != m_mPlayerContextByName.end())
	{
		m_mPlayerContextByName.erase(mit);
	}

	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit2 = m_mPlayerContextBySessionId.find(pPlayerContext->m_iSessionId);
	if (mit2 != m_mPlayerContextBySessionId.end())
	{
		m_mPlayerContextBySessionId.erase(mit2);
	}

	LOG_STT(LOG_SERVER, _T("acc=%s sid=%d Delete player"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
	pPlayerContext->Clear();

	m_PlayerContextPool.Free(pPlayerContext);
}

DWORD MasterServerLoop::_Loop()
{
	while (!m_PlayerFinalizingQueue.empty())
	{
		DeletePlayer(m_PlayerFinalizingQueue.back());
		m_PlayerFinalizingQueue.pop();
	}

	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		if (m_mPlayerContextBySessionId.empty())
		{
			m_iShutdownStatus = READY_FOR_SHUTDOWN;
		}
	}

	return 100;
}

bool MasterServerLoop::_OnCommand(LogicCommand* pCommand)
{
	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return true;
	}

	switch (pCommand->m_iCmdId)
	{
	case COMMAND_SHUTDOWN:
		_OnCommandShutdown();
		break;

	case COMMAND_ONLOGINREQ:
		_OnCommandOnLoginReq((LogicCommandOnLoginReq*)pCommand);
		break;

	case COMMAND_GATEALLOCACK:
		_OnCommandGateAllocAck((LogicCommandGateAllocAck*)pCommand);
		break;

	case COMMAND_ONGATELOGINREQ:
		_OnCommandOnGateLoginReq((LogicCommandOnGateLoginReq*)pCommand);
		break;

	case COMMAND_ONSESSIONDISCONNECT:
		_OnCommandOnSessionDisconnect((LogicCommandOnSessionDisconnect*)pCommand);
		break;

	default:
		break;
	}

	return true;
}

void MasterServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;

	for (stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.begin();
		mit != m_mPlayerContextByName.end(); ++mit)
	{
		ShutdownPlayer(mit->second);
	}
}

void MasterServerLoop::_OnCommandOnLoginReq(LogicCommandOnLoginReq* pCommand)
{
	MasterPlayerContext* pPlayerContext = NULL;
	int32 iRet = 0;

	stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.find(pCommand->m_strAccountName);
	if (mit != m_mPlayerContextByName.end())
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x This account is already logged in"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		pPlayerContext = mit->second;

		iRet = LoginPeerSend::OnLoginAck(g_pServer->m_pLoginServer, pCommand->m_iSessionId, 1);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLoginAck failed"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		}

		ShutdownPlayer(pPlayerContext);
		return;
	}
	
	pPlayerContext = m_PlayerContextPool.Allocate();
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_SERVER, _T("Allocate player context from pool failed, acc=%s, sid=%08x"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		iRet = LoginPeerSend::OnLoginAck(g_pServer->m_pLoginServer, pCommand->m_iSessionId, 2);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLoginAck failed"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		}
		return;
	}

	LOG_DBG(LOG_SERVER, _T("Allocate player context success, acc=%s, sid=%08x"), pCommand->m_strAccountName, pCommand->m_iSessionId);

	m_mPlayerContextByName.insert(std::make_pair(pCommand->m_strAccountName, pPlayerContext));
	m_mPlayerContextBySessionId.insert(std::make_pair(pCommand->m_iSessionId, pPlayerContext));
	m_LoginServerContext.m_mPlayerContext.insert(std::make_pair(pCommand->m_iSessionId, pPlayerContext));

	pPlayerContext->OnLoginReq(pCommand->m_iSessionId, pCommand->m_strAccountName);
}

void MasterServerLoop::_OnCommandGateAllocAck(LogicCommandGateAllocAck* pCommand)
{
	MasterPlayerContext* pPlayerContext = NULL;
	stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.find(pCommand->m_strAccountName);
	if (mit != m_mPlayerContextByName.end())
	{
		pPlayerContext = mit->second;
		if (pPlayerContext->m_iSessionId == pCommand->m_iLoginSessionId)
		{
			pPlayerContext->GateAllocAck(pCommand->m_iServerId, pCommand->m_iGateSessionId);
		}
		else
		{
			LOG_ERR(LOG_SERVER, _T("sessionId is different, sessionId1=%d, sessionId2=%d"), pPlayerContext->m_iSessionId, pCommand->m_iLoginSessionId);
		}
	}
	else
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s does not exist in master server"), pCommand->m_strAccountName);
	}
}

void MasterServerLoop::_OnCommandOnGateLoginReq(LogicCommandOnGateLoginReq* pCommand)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit == m_mPlayerContextBySessionId.end())
	{
		LOG_WAR(LOG_SERVER, _T("acc=? sid=%d can't find context"), pCommand->m_iSessionId);
		return;
	}

	mit->second->OnGateLoginReq();
}

void MasterServerLoop::_OnCommandOnSessionDisconnect(LogicCommandOnSessionDisconnect* pCommand)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit != m_mPlayerContextBySessionId.end())
	{
		LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive disconnect from session server"), mit->second->m_strAccountName, pCommand->m_iSessionId);
		mit->second->OnSessionDisconnect();
	}
}